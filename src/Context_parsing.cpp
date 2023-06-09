#include <Context.hpp>
#include <Client.hpp>
#include <Handler.hpp>
#include <ircserv.hpp>
#include <sstream>

std::vector<std::string>	splitByChar(const std::string &string, char spliter)
{
	std::istringstream			ss(string);
	std::vector<std::string>	seggies;
	std::string					seggie;

	while (!ss.eof())
	{
		std::getline(ss, seggie, spliter);
		seggies.push_back(seggie);
	}
	if (seggies.back().find_first_of("\r\n") != std::string::npos)
		seggies.back() = seggies.back().substr(0, seggies.back().size() - 1);
	return (seggies);
}

std::string	joinVectorStrings(std::vector<std::string> &vec)
{
	std::string	result;

	for (std::vector<std::string>::iterator i = vec.begin(); i != vec.end(); ++i)
		result += *i;
	return (result);
}

// if (it->find("QUIT") != std::string::npos)
// {
// 	server->closeConection(client->getId());
// 	break ;
// }
// else if (it->find("JOIN #", 0) != std::string::npos)
// {

// }
// else if (it->find("PRIVMSG ", 0) != std::string::npos)
// {
// 	//must go with # if channel
// 	std::string recipient = it->substr(8, it->find(" :", 8) - 8);
// 	//msg body after :
// 	std::string msg = it->substr(it->find(" :", 8) + 2, it->length() - it->find(" :", 8) - 3);
// 	cmd_sendPM(client->getId(), recipient, msg);
// }
// else if (it->find("MODE ", 0) != std::string::npos)
// {
// 	std::vector<Channel>::iterator channel = find_chan_by_name(it->substr(5, it->length() - 6));
// 	if (isChannelInVector(channel))
// 		RPL_CHANNELMODEIS(client->getId(), *channel);
// }
// else if (it->find("WHO ", 0) != std::string::npos)
// {
// 	std::vector<Channel>::iterator channel = find_chan_by_name(it->substr(5, it->length() - 6));
// 	if (isChannelInVector(channel))
// 		RPL_WHOREPLY(client->getId(), *channel);
// 	else
// 		ERR_NOSUCHCHANNEL(client->getId(), it->substr(5, it->length() - 6));	
// }
void	Context::execClientCmds(int id)
{
	std::vector<Client>::iterator		client = find_client_by_id(id); 
	std::vector<std::string>			&cmds = client->getCmds();
	std::string							cmdStr = joinVectorStrings(cmds), buf, options[] = {"INVITE", "NICK", "TOPIC", "LIST", "KICK", "JOIN", "QUIT", "PRIVMSG", "MODE", "WHO"};
	int									i;

	std::istringstream			cmd(cmdStr);
	if (cmd.fail())
		return (ERR_UNRECOGNIZEDCMD(client->getId(), cmdStr, "Failed to allocate string"));
	std::getline(cmd, buf, ' ');
	for (i = 0; i < 10; ++i)
		if (options[i] == buf)
			break ;
	switch(i)
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
		default:
			return (ERR_UNRECOGNIZEDCMD(client->getId(), buf, "Unrecognized command"));
	}




	//INVITE USER CHANNEL	- invite
	//NICK <nick>			- set nick - all sharacters
	//TOPIC <channel> :<topic>, if no topic but : - set topic to "" - if no topic and no : - show topic
	//LIST - show channels
	//KICK <channel> <tarjet> :<reason>
	//JOIN #<CNAME1> #<CNAME2> #<CNAMEn>
	//QUIT :<REASON>
	//PRIVMSG <channel-pub/user-priv> :msg
	//MODE +<FLAG><<PARAMS>> - can or not be space separated - will trim whitespaces - +it can be together rest alone +k neext <pass> +l needs<limit> +o needs<target>
	//WHO #channel
}		
void	Context::parseJoin(std::vector<Client>::iterator client, std::string &cmd) 
{
	std::vector<std::string>	seggies = splitByChar(cmd, ' ');

	if (seggies.size() != 2)
		return (ERR_UNRECOGNIZEDCMD(client->getId(), seggies[0], "Unrecognized command"));
	cmd_join( client->getId(), seggies[1], "");
}

void	Context::parseInvite(std::vector<Client>::iterator client, std::string &cmd) 
{
	(void)client;
	(void)cmd;

}

void	Context::parseWho(std::vector<Client>::iterator client, std::string &cmd) 
{
	std::vector<Channel>::iterator	chan;
	std::vector<std::string>		seggies = splitByChar(cmd, ' ');
	std::string						cleanChan;

	if (seggies.size() != 2)
		return (ERR_UNRECOGNIZEDCMD(client->getId(), seggies[0], "Unrecognized command"));
	cleanChan = seggies[1].substr(1, seggies[1].size() - 1);
	chan = find_chan_by_name(cleanChan);
	if (!isChannelInVector(chan))
		return ((void)ERR_NOSUCHCHANNEL(client->getId(), cleanChan));
	RPL_WHOREPLY( client->getId(), *chan);
}

void	Context::parseMode(std::vector<Client>::iterator client, std::string &cmd) 
{
	std::vector<std::string>		seggies = splitByChar(cmd, ' ');
	std::vector<Channel>::iterator	chan;
	std::string						cleanChan;

	if (seggies.back().empty())
		seggies.pop_back();
	if (seggies.size() < 2)
		return (ERR_UNRECOGNIZEDCMD(client->getId(), seggies[0], "USAGE: MODE #channel +/-option <parameters>"));
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
}

void	Context::parseNick(std::vector<Client>::iterator client, std::string &cmd) 
{
	(void)client;
	(void)cmd;

}

void	Context::parseTopic(std::vector<Client>::iterator client, std::string &cmd) 
{
	(void)client;
	(void)cmd;

}

void	Context::parseList(std::vector<Client>::iterator client, std::string &cmd) 
{
	(void)client;
	(void)cmd;

}

void	Context::parseKick(std::vector<Client>::iterator client, std::string &cmd) 
{
	(void)client;
	(void)cmd;

}

void	Context::parsePrivmsg(std::vector<Client>::iterator client, std::string &cmd) 
{
	(void)client;
	(void)cmd;

}
void	Context::parseQuit(std::vector<Client>::iterator client, std::string &cmd) 
{
	(void)cmd;
	server->closeConection(client->getId());
}

bool	Context::verifyModeOptions(std::vector<std::string> &vec)
{
	std::string	opts;

	if (vec[2][0] != '+' && vec[2][0] != '-')
		return (false);
	opts = vec[2].substr(1, vec[2].size() - 1);
	if (vec[2][0] == '+')
	{
		if ((!opts.compare("it") || !opts.compare("ti") || !opts.compare("i") || !opts.compare("t"))
			&& vec.size() == 3)
			return (true);
		if ((!opts.compare("o") && vec.size() > 3 ) || (!opts.compare("k") && vec.size() == 4)
			|| (!opts.compare("l") && vec.size() == 4 && isNumeric(vec[3])))
			return(true);
		return (false);
	}
	if (vec[2][0] == '-')
	{
		if (opts.find_first_not_of("itkl") == std::string::npos && vec.size() == 3)
			return (true);
		if ((!opts.compare("o") && vec.size() > 3 ) || (!opts.compare("k") && vec.size() == 4)
			|| (!opts.compare("l") && vec.size() == 4 && isNumeric(vec[3])))
			return(true);
		return (false);
	}
	return (false);
}
