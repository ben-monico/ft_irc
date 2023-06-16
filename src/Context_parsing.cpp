#include <Context.hpp>
#include <Client.hpp>
#include <Channel.hpp>
#include <Handler.hpp>
#include <ircserv.hpp>
#include <sstream>

std::vector<std::string> splitByChar(const std::string &string, char spliter)
{
	std::istringstream ss(string);
	std::vector<std::string> seggies;
	std::string seggie;

	while (!ss.eof())
	{
		std::getline(ss, seggie, spliter);
		if (seggie.size())
			seggies.push_back(seggie);
	}
	if (seggies.back().find("\r") != std::string::npos)
		seggies.back() = seggies.back().substr(0, seggies.back().size() - 1);
	return (seggies);
}

std::string joinVectorStrings(std::vector<std::string> &vec)
{
	std::string result;

	for (std::vector<std::string>::iterator i = vec.begin(); i != vec.end(); ++i)
		result += *i + (i + 1 == vec.end() ? "" : "  ");
	return (result);
}
void	Context::joinPartialCmdStrings(std::vector<std::string>	&cmds)
{
	std::vector<std::string>	joinedCmds;
	std::string					cmd = "";

	for (std::vector<std::string>::iterator i = cmds.begin(); i != cmds.end(); ++i)
	{
		if (cmd.find("\r") == std::string::npos || i->find("\r") == std::string::npos)
			cmd += *i;
		if (cmd.find("\r") != std::string::npos)
		{
			joinedCmds.push_back(cmd);
			cmd = "";
		}
	}
	cmds.erase(cmds.begin(), cmds.end());
	cmds.reserve(joinedCmds.size());
	cmds.insert(cmds.begin(), joinedCmds.begin(), joinedCmds.end());
}

//TODO:
// MODE +<FLAG><<PARAMS>> - can or not be space separated - will trim whitespaces
	// - +it can be together rest alone +k neext <pass> +l needs<limit> +o needs<target> ✅
// NICK <nick>			- set nick - all sharacters ✅
// TOPIC <channel> :<topic>, if no topic but : - set topic to "" - if no topic and no : - show topic ✅
// JOIN #<CNAME1> #<CNAME2> #<CNAMEn> ✅
// QUIT :<REASON> ✅
// WHO #channel ✅
// PART #channel :<reason>  ✅
// KICK <channel> <tarjet> :<reason> ✅
// INVITE <user> #<channel> ✅
// PRIVMSG <channel-pub/user-priv> :msg ✅
void Context::execClientCmds(int id)
{
	std::vector<Client>::iterator client = findClientByID(id);
	int i;
	std::vector<std::string> &cmds = client->getCmds();
	std::string cmdStr, buf, options[] = {"INVITE", "NICK", "TOPIC", "KICK", "JOIN", "QUIT", "PRIVMSG", "MODE", "WHO", "PART"};
	
	joinPartialCmdStrings(cmds);
	cmdStr = cmds[0];

	// for (std::vector<std::string>::iterator i = client->getCmds().begin(); i !=  client->getCmds().end(); ++i)
	// 	std::cout << "client cmds after join " << client->getCmds().size() - (client->getCmds().end() - i) << " = " << *i << std::endl;
	std::istringstream cmd(cmdStr);
	if (cmd.fail())
		return (ERR_UNRECOGNIZEDCMD(client->getID(), cmdStr, "Failed to allocate string"));
	std::getline(cmd, buf, ' ');
	for (i = 0; i < 10; ++i)
		if (options[i] == buf)
			break;
	switch (i)
	{
	case 0:
		parseInvite(client, cmdStr);
		break;
	case 1:
		parseNick(client, cmdStr);
		break;
	case 2:
		parseTopic(client, cmdStr);
		break;
	case 3:
		parseKick(client, cmdStr);
		break;
	case 4:
		parseJoin(client, cmdStr);
		break;
	case 5:
		parseQuit(client, cmdStr);
		return;
	case 6:
		parsePrivmsg(client, cmdStr);
		break;
	case 7:
		parseMode(client, cmdStr);
		break;
	case 8:
		parseWho(client, cmdStr);
		break;
	case 9:
		parsePart(client, cmdStr);
		break;
	default:
		ERR_UNRECOGNIZEDCMD(client->getID(), buf, "Unrecognized command");
	}
	cmds.erase(cmds.begin());
	if (cmds.size())
		execClientCmds(id);
}

void Context::parseJoin(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string> seggies = splitByChar(cmd, ' ');

	if (seggies.size() < 2 || seggies.size() > 3)
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /JOIN #<channel> <<key>>"));
	if (seggies[1][0] != '#' || (seggies[1][0] == '#' && !isNickValid(seggies[1].substr(1, seggies[1].size() - 1))))
		return (ERR_NOSUCHCHANNEL(client->getID(), seggies[1]));
	if (seggies.size() == 2)
		return (cmd_join(client->getID(), seggies[1].substr(1, seggies[1].size() - 1), ""));
	cmd_join(client->getID(), seggies[1].substr(1, seggies[1].size() - 1), seggies[2]);
}

void Context::parseInvite(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>		seggies = splitByChar(cmd, ' ');
	std::vector<Channel>::iterator	channo;
	std::vector<Client>::iterator	target;
	std::string						cleanChan;
	
	if (seggies.size() != 3)
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /INVITE <target> #<channel>"));
	target = find_client_by_nick(seggies[1]);
	if (!isClientInVector(target))
		return (ERR_NOSUCHNICK(client->getID(), seggies[1]));
	cleanChan = seggies[2].substr(1, seggies[2].size() - 1);
	channo = findChannelByName(cleanChan);
	if (!isChannelInVector(channo))
		return (ERR_NOSUCHCHANNEL(client->getID(), cleanChan));
	if (channo->isUserInChannel(target->getID()))
		return (ERR_USERONCHANNEL(client->getID(), seggies[1], cleanChan));
	chanop_inviteUser(client->getID(), cleanChan, target->getNick());
}

void Context::parseWho(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<Channel>::iterator chan;
	std::vector<std::string> seggies = splitByChar(cmd, ' ');
	std::string cleanChan;

	if (seggies.size() != 2)
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /WHO #<channel>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	chan = findChannelByName(cleanChan);
	if (!isChannelInVector(chan))
		return (ERR_NOSUCHCHANNEL(client->getID(), cleanChan));
	RPL_WHOREPLY(client->getID(), *chan);
}

void Context::parseMode(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>		seggies = splitByChar(cmd, ' ');
	std::vector<Channel>::iterator	chan;
	std::string						cleanChan;

	if (seggies.back().empty())
		seggies.pop_back();
	if (seggies.size() < 2)
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /MODE +/-<options> <<params>>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	if (seggies.size() > 2 && verifyModeOptions(seggies))
		execModeOptions(seggies, client, cleanChan);
	else if (seggies.size() == 2)
	{
		chan = findChannelByName(cleanChan);
		if (!isChannelInVector(chan))
			return (ERR_NOSUCHCHANNEL(client->getID(), cleanChan));
		RPL_CHANNELMODEIS(client->getID(), *chan);
	}
	else 
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /MODE +/-<options> <<params>>"));
}

void Context::parseNick(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string> seggies = splitByChar(cmd, ' ');

	if (seggies.size() != 2 || (seggies.size() == 2 && seggies[1].empty()))
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /NICK <nick>"));
	Context::cmd_setNick(client->getID(), seggies[1]);
}

void Context::parseTopic(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>		seggies = splitByChar(cmd, ' ');
	std::string						cleanChan, topic;
	std::vector<Channel>::iterator	channo;

	if (seggies.size() < 2)
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /TOPIC #<channel> :<topic string>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	channo = findChannelByName(cleanChan);
	if (!isChannelInVector(channo))
		return (ERR_NOSUCHCHANNEL(client->getID(), cleanChan));
	if (seggies.size() == 2)
		return (RPL_TOPIC(client->getID(), *channo));
	if (seggies[2][0] != ':')
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /TOPIC #<channel> :<topic string>"));
	seggies.erase(seggies.begin(), seggies.begin() + 2);
	topic = joinVectorStrings(seggies);
	return (chanop_topic(client->getID(), cleanChan, topic[0] ? topic.substr(1, topic.size() - 1) : ""));
}

void Context::parseKick(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>				seggies = splitByChar(cmd, ' ');
	std::vector<Channel>::iterator			channo;
	std::vector<Client>::iterator			target;
	std::string								cleanChan, reason;

	if (seggies.size() < 3)
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /KICK #<channel> :<reason>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	channo = findChannelByName(cleanChan);
	if (!isChannelInVector(channo))
		return (ERR_NOSUCHCHANNEL(client->getID(), cleanChan));
	target = find_client_by_nick(seggies[2]);
	if (!isClientInVector(target))
		return (ERR_NOSUCHNICK(client->getID(), seggies[2]));
	if (!channo->isUserInChannel(target->getID()))
		return (ERR_USERNOTINCHANNEL(client->getID(), cleanChan, seggies[2]));
	if (seggies.size() >= 4)
	{
		seggies.erase(seggies.begin(), seggies.begin() + 3);
		reason = joinVectorStrings(seggies);
		if (reason[0] != ':')
			return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /KICK #<channel> :<reason>"));
		reason = reason.substr(1, reason.size() - 1);
	}
	else
		reason = "no reason specified.";
	chanop_kickUser(client->getID(), cleanChan, target->getNick(), reason);
}

void Context::parsePrivmsg(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>	seggies = splitByChar(cmd, ' ');
	std::string					target, msg;

	if(seggies.size() < 3)
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /PRIVMSG #<channel>/<target> :<msg>"));
	target = seggies[1];
	seggies.erase(seggies.begin(), seggies.begin() + 2);
	msg = joinVectorStrings(seggies);
	if (msg[0] != ':')
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /PRIVMSG #<channel>/<target> :<msg>"));
	msg = msg.substr(1, msg.size() - 1);
	cmd_sendPM(client->getID(), target, msg);

}

void Context::parseQuit(std::vector<Client>::iterator client, std::string &cmd)
{
	std::string reason = cmd.substr(5, cmd.size() - 5);
	if (reason.empty() || reason[0] != ':')
		reason = "no reason specified.";
	cmd_quit(client->getID(), reason.substr(1, reason.size() - 1));
}

void Context::parsePart(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>		seggies = splitByChar(cmd, ' ');
	std::vector<Channel>::iterator	channo;
	std::string						cleanChan, reason;

	if (seggies.size() < 3)
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /PART #<channel> :<reason>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	channo = findChannelByName(cleanChan);
	if (!isChannelInVector(channo))
		return (ERR_NOSUCHCHANNEL(client->getID(), cleanChan));
	if (!channo->isUserInChannel(client->getID()))
		return (ERR_USERNOTINCHANNEL(client->getID(), cleanChan, seggies[2]));
	seggies.erase(seggies.begin(), seggies.begin() + 2);
	reason = joinVectorStrings(seggies);
	if (reason[0] != ':')
		return (ERR_NEEDMOREPARAMS(client->getID(), "USAGE: /PART #<channel> :<reason>"));
	cmd_part(client->getID(), cleanChan, reason[0] ? reason.substr(1, reason.size() - 1) : "");
}


bool Context::verifyModeOptions(std::vector<std::string> &vec)
{
	std::string opts;

	if (vec[2][0] != '+' && vec[2][0] != '-')
		return (false);
	opts = vec[2].substr(1, vec[2].size() - 1);
	if (vec[2][0] == '+')
	{
		if ((!opts.compare("it") || !opts.compare("ti") || !opts.compare("i") || !opts.compare("t")) && vec.size() == 3)
			return (true);
		if ((!opts.compare("o") && vec.size() > 3) || (!opts.compare("k") && vec.size() <= 4) || (!opts.compare("l") && vec.size() == 4 && isNumeric(vec[3])))
			return (true);
		return (false);
	}
	if (vec[2][0] == '-')
	{
		if (opts.find_first_not_of("itkl") == std::string::npos && vec.size() == 3)
			return (true);
		if ((!opts.compare("o") && vec.size() > 3) || (!opts.compare("k") && vec.size() == 4) || (!opts.compare("l") && vec.size() == 4 && isNumeric(vec[3])))
			return (true);
		return (false);
	}
	return (false);
}
