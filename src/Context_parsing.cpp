#include <Context.hpp>
#include <Client.hpp>
#include <Channel.hpp>
#include <Handler.hpp>
#include <ircserv.hpp>
#include <sstream>

//'alfanum' '-(/)_' ''

std::vector<std::string> splitByChar(const std::string &string, char spliter)
{
	std::istringstream ss(string);
	std::vector<std::string> seggies;
	std::string seggie;

	while (!ss.eof())
	{
		std::getline(ss, seggie, spliter);
		seggies.push_back(seggie);
	}
	if (seggies.back().find_first_of("\r\n") != std::string::npos)
		seggies.back() = seggies.back().substr(0, seggies.back().size() - 1);
	return (seggies);
}

std::string joinVectorStrings(std::vector<std::string> &vec)
{
	std::string result;

	for (std::vector<std::string>::iterator i = vec.begin(); i != vec.end(); ++i)
		result += *i;
	return (result);
}
void	Context::joinPartialCmdStrings(std::vector<Client>::iterator client)
{
	std::vector<std::string>	&cmds = client->getCmds();
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
void Context::execClientCmds(int id)
{
	std::vector<Client>::iterator client = find_client_by_id(id);
	joinPartialCmdStrings(client);
	std::vector<std::string> &cmds = client->getCmds();
	std::string cmdStr = joinVectorStrings(cmds), buf, options[] = {"INVITE", "NICK", "TOPIC", "LIST", "KICK", "JOIN", "QUIT", "PRIVMSG", "MODE", "WHO", "PART"};
	int i;

	std::istringstream cmd(cmdStr);
	if (cmd.fail())
		return (ERR_UNRECOGNIZEDCMD(client->getId(), cmdStr, "Failed to allocate string"));
	std::getline(cmd, buf, ' ');
	for (i = 0; i < 11; ++i)
		if (options[i] == buf)
			break;
	std::cout << "OPTION = " << i << std::endl;
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
		parseList(client, cmdStr);
		break;
	case 4:
		parseKick(client, cmdStr);
		break;
	case 5:
		parseJoin(client, cmdStr);
		break;
	case 6:
		parseQuit(client, cmdStr);
		break;
	case 7:
		parsePrivmsg(client, cmdStr);
		break;
	case 8:
		parseMode(client, cmdStr);
		break;
	case 9:
		parseWho(client, cmdStr);
		break;
	case 10:
		parsePart(client, cmdStr);
		break;
	default:
		return (ERR_UNRECOGNIZEDCMD(client->getId(), buf, "Unrecognized command"));
	}
	//TODO:
	// MODE +<FLAG><<PARAMS>> - can or not be space separated - will trim whitespaces
		// - +it can be together rest alone +k neext <pass> +l needs<limit> +o needs<target> ✅
	// INVITE USER CHANNEL	- invite ✅
	// NICK <nick>			- set nick - all sharacters ✅
	// TOPIC <channel> :<topic>, if no topic but : - set topic to "" - if no topic and no : - show topic ✅
	// JOIN #<CNAME1> #<CNAME2> #<CNAMEn> ✅
	// QUIT :<REASON> ✅
	// WHO #channel ✅
	// KICK <channel> <tarjet> :<reason> ✅
	// LIST - show channels --- missing bronado --- 
	// PART #channel :<reason> --- missing bronado --- 
	// PRIVMSG <channel-pub/user-priv> :msg
}

void Context::parseJoin(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string> seggies = splitByChar(cmd, ' ');

	if (seggies.size() < 2 || seggies.size() > 3)
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + " #<channel> <<key>>"));
	if (seggies.size() == 2)
		return (cmd_join(client->getId(), seggies[1], ""));
	cmd_join(client->getId(), seggies[1], seggies[2]);
}

void Context::parseInvite(std::vector<Client>::iterator client, std::string &cmd)
{
	(void)client;
	(void)cmd;
}

void Context::parseWho(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<Channel>::iterator chan;
	std::vector<std::string> seggies = splitByChar(cmd, ' ');
	std::string cleanChan;

	if (seggies.size() != 2)
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + " #<channel>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	chan = find_chan_by_name(cleanChan);
	if (!isChannelInVector(chan))
		return (ERR_NOSUCHCHANNEL(client->getId(), cleanChan));
	RPL_WHOREPLY(client->getId(), *chan);
}

void Context::parseMode(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>		seggies = splitByChar(cmd, ' ');
	std::vector<Channel>::iterator	chan;
	std::string						cleanChan;

	if (seggies.back().empty())
		seggies.pop_back();
	if (seggies.size() < 2)
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + " +/-<options> <<params>>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	if (seggies.size() > 2 && verifyModeOptions(seggies))
		execModeOptions(seggies, client, cleanChan);
	else if (seggies.size() == 2)
	{
		chan = find_chan_by_name(cleanChan);
		if (!isChannelInVector(chan))
			return (ERR_NOSUCHCHANNEL(client->getId(), cleanChan));
		RPL_CHANNELMODEIS(client->getId(), *chan);
	}
	else 
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + " +/-<options> <<params>>"));
}

void Context::parseNick(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string> seggies = splitByChar(cmd, ' ');

	if (seggies.size() != 2 || (seggies.size() == 2 && seggies[1].empty()))
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + " <nick>"));
	Context::cmd_setNick(client->getId(), seggies[1]);
}

void Context::parseTopic(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>		seggies = splitByChar(cmd, ' ');
	std::string						cleanChan, topic;
	std::vector<Channel>::iterator	channo;

	if (seggies.size() < 2)
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + "#<channel> :<topic string>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	channo = find_chan_by_name(cleanChan);
	if (!isChannelInVector(channo))
		return (ERR_NOSUCHCHANNEL(client->getId(), cleanChan));
	if (seggies.size() == 2)
		return (RPL_TOPIC(client->getId(), *channo));
	if (seggies[2][0] != ':')
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + "#<channel> :<topic string>"));
	seggies.erase(seggies.begin(), seggies.begin() + 2);
	topic = joinVectorStrings(seggies);
	return (chanop_topic(client->getId(), cleanChan, topic[0] ? topic.substr(1, topic.size() - 1) : ""));
}

void Context::parseList(std::vector<Client>::iterator client, std::string &cmd)
{
	(void) client;
	(void) cmd;
}

void Context::parseKick(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>				seggies = splitByChar(cmd, ' ');
	std::vector<Channel>::iterator			channo;
	std::vector<Client>::iterator			target;
	std::string								cleanChan, reason;

	if (seggies.size() < 4)
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + "#<channel> :<reason>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	channo = find_chan_by_name(cleanChan);
	if (!isChannelInVector(channo))
		return (ERR_NOSUCHCHANNEL(client->getId(), cleanChan));
	target = find_client_by_nick(seggies[2]);
	if (!isUserInVector(target))
		return (ERR_NOSUCHNICK(client->getId(), seggies[2]));
	if (!channo->isUserInChannel(target->getId()))
		return (ERR_USERNOTINCHANNEL(client->getId(), cleanChan, seggies[2]));
	seggies.erase(seggies.begin(), seggies.begin() + 3);
	reason = joinVectorStrings(seggies);
	if (reason[0] != ':')
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + "#<channel> :<reason>"));
	chanop_kickUser(client->getId(), cleanChan, target->getNick(), reason);
}

void Context::parsePrivmsg(std::vector<Client>::iterator client, std::string &cmd)
{
	(void)client;
	(void)cmd;
}
void Context::parseQuit(std::vector<Client>::iterator client, std::string &cmd)
{
	(void)cmd;
	server->closeConection(client->getId());
}

void Context::parsePart(std::vector<Client>::iterator client, std::string &cmd)
{
	std::vector<std::string>		seggies = splitByChar(cmd, ' ');
	std::vector<Channel>::iterator	channo;
	std::string						cleanChan, reason;

	if (seggies.size() != 3)
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + "#<channel> :<reason>"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	channo = find_chan_by_name(cleanChan);
	if (!isChannelInVector(channo))
		return (ERR_NOSUCHCHANNEL(client->getId(), cleanChan));
	if (!channo->isUserInChannel(client->getId()))
		return (ERR_USERNOTINCHANNEL(client->getId(), cleanChan, seggies[2]));
	seggies.erase(seggies.begin(), seggies.begin() + 2);
	reason = joinVectorStrings(seggies);
	if (reason[0] != ':')
		return (ERR_NEEDMOREPARAMS(client->getId(), seggies[0], "USAGE: " + seggies[0] + "#<channel> :<reason>"));
	cmd_part(client->getId(), cleanChan, reason[0] ? reason.substr(1, reason.size() - 1) : "");
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
