#include <Context.hpp>
#include <Channel.hpp>
#include <Handler.hpp>

//:ircserv 332 userNick #channel :Topic of the channel
void Context::CHAN_RPL_TEMPLATE(std::string code, Channel &channel, std::string msg, int client_id)
{
	server->sendAllBytes(_hostname + code + " * " + \
	find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " :" \
	+ msg + "\r\n", client_id);
}

void Context::USR_RPL_TEMPLATE(std::string code, std::string msg, std::string nick, int client_id)
{
	server->sendAllBytes(_hostname + code + " * " + nick + " :" \
	+ msg + "\r\n", client_id);
}

void Context::RPL_WELCOME(int client_id)
{
	server->sendAllBytes(_hostname + "CAP " + find_client_by_id(client_id)->getNick() + "NAK :302\r\n", client_id);
	server->sendAllBytes(_hostname + "001 " + find_client_by_id(client_id)->getNick() + " :Welcome to Hell, " + find_client_by_id(client_id)->getNick() + "\r\n", client_id);
	server->sendAllBytes(_hostname + "005 " + find_client_by_id(client_id)->getNick() + " CHANTYPES=#\r\n", client_id);
	server->sendAllBytes(_hostname + "005 " + find_client_by_id(client_id)->getNick() + " CHANMODES=i,t,k,o,l\r\n", client_id);
}

void Context::RPL_CHANNELMODEIS(int client_id, Channel &channel)
{
	server->sendAllBytes(_hostname + "324 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + "\r\n", client_id);
}

void Context::RPL_NOTOPIC(int client_id, Channel &channel)
{
	server->sendAllBytes(_hostname + "331 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " :" \
	+ "No topic is set.\r\n", client_id);
}

void Context::RPL_TOPIC(int client_id, Channel &channel)
{
	if (channel.getTopic().empty())
		return RPL_NOTOPIC(client_id, channel);
	server->sendAllBytes(_hostname + "332 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " :" \
	+ channel.getTopic() + "\r\n", client_id);
}

void Context::RPL_WHOREPLY(int client_id, Channel &channel)
{
	std::string msg = "";
	std::vector<Client>::iterator it = _clients.begin();
	for (; it != _clients.end(); ++it)
	{
		if (it->isOnChannel(channel.getName()))
			server->sendAllBytes(_hostname + "352 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " " + \
			it->getUserName() + " ircserv " + it->getNick() + " H :0 " + msg + "\r\n", client_id);
	}
	server->sendAllBytes(_hostname + "352 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " " + \
	_hostname + " " + _hostname + " " + find_client_by_id(client_id)->getNick() + " H :0 " + msg + "\r\n", client_id);
}

void Context::RPL_NAMREPLY(int client_id, Channel &channel)
{
	std::string msg = "";
	std::vector<Client>::iterator it = _clients.begin();
	for (; it != _clients.end(); ++it)
	{
		if (it->isOnChannel(channel.getName()))
			msg += it->getChannelMode(channel.getName()) + it->getNick() + " ";
	}
	channel.broadcastMsg(_hostname + "353 " + find_client_by_id(client_id)->getNick() + " = #" + channel.getName() + " :" \
	+ msg + "\r\n", server, -1);
}

void Context::RPL_ENDOFNAMES(int client_id, Channel &channel)
{
	server->sendAllBytes(_hostname + "366 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " :End of /NAMES list.\r\n", client_id);
}

void Context::ERR_NOSUCHNICK(int client_id, std::string const& nick)
{
	server->sendAllBytes(_hostname + "401 " + find_client_by_id(client_id)->getNick() + " " + nick + " :No such nick\r\n", client_id);
}

void Context::ERR_NOSUCHCHANNEL(int client_id, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "403 " + find_client_by_id(client_id)->getNick() + " #" + channel_name + " :No such channel\r\n", client_id);
}

void Context::ERR_NOTONCHANNEL(int client_id, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "442 " + find_client_by_id(client_id)->getNick() + " #" + channel_name + " :User not on channel\r\n", client_id);
}

void Context::ERR_USERONCHANNEL(int client_id, std::string const& nick, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "443 " + find_client_by_id(client_id)->getNick() + " " + nick + " #" + channel_name + " :is already on channel\r\n", client_id);
}

void Context::ERR_PASSWDMISMATCH(int client_id)
{
	server->sendAllBytes(_hostname + "464 :Password Mismatched password. (Connection Refused)\r\n", client_id);
}

void Context::ERR_CHANNELISFULL(int client_id, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "471 " + find_client_by_id(client_id)->getNick() + " #" + channel_name + " :Cannot join channel (+l)\r\n", client_id);
}

void Context::ERR_INVITEONLYCHAN(int client_id, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "473 " + find_client_by_id(client_id)->getNick() + " #" + channel_name + " :Cannot join channel (+i)\r\n", client_id);
}

void Context::ERR_BADCHANNELKEY(int client_id, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "475 " + find_client_by_id(client_id)->getNick() + " #" + channel_name + " :Cannot join channel (+k)\r\n", client_id);
}

void Context::ERR_CHANOPRIVSNEEDED(int client_id, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "482 " + find_client_by_id(client_id)->getNick() + " #" + channel_name + " :You're not channel operator\r\n", client_id);
}