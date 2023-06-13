#include <Context.hpp>
#include <Channel.hpp>
#include <Handler.hpp>

void Context::RPL_WELCOME(int client_id)
{
	server->sendAllBytes(_hostname + "001 " + find_client_by_id(client_id)->getNick() + " :Welcome to Hell, " + find_client_by_id(client_id)->getNick() + "\r\n", client_id);
	server->sendAllBytes(_hostname + "005 " + find_client_by_id(client_id)->getNick() + " CHANTYPES=#\r\n", client_id);
	server->sendAllBytes(_hostname + "005 " + find_client_by_id(client_id)->getNick() + " CHANMODES=i,t,k,o,l\r\n", client_id);
}

void Context::RPL_CHANNELMODEIS(int client_id, Channel &channel)
{
	channel.getModes(client_id, server, _hostname);
}

void Context::RPL_NOTOPIC(int client_id, Channel &channel)
{
	channel.broadcastMsg(_hostname + "331 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " :" \
	+ "No topic is set.\r\n", server, -1);
}

void Context::RPL_TOPIC(int client_id, Channel &channel)
{
	if (channel.getTopic().empty())
		return RPL_NOTOPIC(client_id, channel);
	channel.broadcastMsg(_hostname + "332 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " :" \
	+ channel.getTopic() + "\r\n", server, -1);
}

void Context::RPL_WHOREPLY(int client_id, Channel &channel)
{
	std::string msg = "";
	std::vector<int>::iterator it = channel._usersID.begin();
	for (; it != channel._usersID.end(); ++it)
	{
		std::vector<Client>::iterator user = find_client_by_id(*it);
		server->sendAllBytes(_hostname + "352 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " " + \
			user->getNick() + " " + user->getUserName() + " localhost ircserv " + user->getNick() + " " + user->getChannelMode(channel.getName()) + " :0 realname\r\n", client_id);
	}
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
	channel.broadcastMsg(_hostname + "366 " + find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " :End of /NAMES list.\r\n", server, -1);
}

void Context::ERR_NOSUCHNICK(int client_id, std::string const& nick)
{
	server->sendAllBytes(_hostname + "401 " + find_client_by_id(client_id)->getNick() + " " + nick + " :No such nick\r\n", client_id);
}

void Context::ERR_NOSUCHCHANNEL(int client_id, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "403 " + find_client_by_id(client_id)->getNick() + " #" + channel_name + " :No such channel\r\n", client_id);
}

void Context::ERR_CANNOTSENDTOCHAN(int client_id, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "404 " + find_client_by_id(client_id)->getNick() + " #" + channel_name + " :Cannot send to channel\r\n", client_id);
}

void	Context::ERR_UNRECOGNIZEDCMD(const int &client_id, const std::string &cmd, const std::string reason)
{
	server->sendAllBytes(_hostname + "421 " + find_client_by_id(client_id)->getNick() + " " + cmd + " :" + reason + "\r\n",  client_id);
}

void	Context::ERR_NICKNAMEINUSE(int client_id, const std::string &nick)
{
	server->sendAllBytes(_hostname + "433 " + nick + " " + nick + " :Nickname is already in use\r\n",  client_id);
}

void Context::ERR_NOTONCHANNEL(int client_id, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "442 " + find_client_by_id(client_id)->getNick() + " #" + channel_name + " :User not on channel\r\n", client_id);
}

void Context::ERR_USERONCHANNEL(int client_id, std::string const& nick, std::string const& channel_name)
{
	server->sendAllBytes(_hostname + "443 " + find_client_by_id(client_id)->getNick() + " " + nick + " #" + channel_name + " :is already on channel\r\n", client_id);
}

void Context::ERR_PASSWDMISMATCH(int client_id, const std::string &nick)
{
	server->sendAllBytes(_hostname + "464 " + nick + " :Password incorrect\r\n", client_id);
	server->closeConection(client_id);
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

void	Context::ERR_ERRONEUSNICKNAME( int client_id, const std::string &nick)
{
	server->sendAllBytes(_hostname + "432 " + nick + " " + nick + " :Erroneus nickname" + "\r\n", client_id);
}

void	Context::ERR_NEEDMOREPARAMS( int client_id, const std::string cmd, const std::string reason)
{
	server->sendAllBytes(_hostname + "461 " + cmd + " :" + reason + "\r\n", client_id);
}

void	Context::ERR_USERNOTINCHANNEL( const int &client_id, const std::string &chan, const std::string &nick)
{
	server->sendAllBytes(_hostname + "441 " + nick + " " + chan + " :Target not in channel\r\n", client_id);
}
