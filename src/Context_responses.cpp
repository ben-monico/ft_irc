#include <Context.hpp>
#include <Channel.hpp>
#include <Handler.hpp>

//:ircserv 332 userNick #channel :Topic of the channel
void Context::RPL_TEMPLATE(std::string code, Channel &channel, std::string msg, int client_id)
{
	server->sendAllBytes(_hostname + code + " " + \
	find_client_by_id(client_id)->getNick() + " #" + channel.getName() + " :" \
	+ msg + "\r\n", client_id);
}

void Context::RPL_TOPIC(int client_id, Channel &channel)
{
	RPL_TEMPLATE("332", channel, channel.getTopic(), client_id);
}

void Context::RPL_NAMREPLY(int client_id, Channel &channel)
{
	std::string msg = "";
	std::vector<Client>::iterator it = channel.getClients().begin();
	for (; it != channel.getClients().end(); ++it)
		msg += it->getChannelMode(channel.getName()) + it->getNick() + " ";
	RPL_TEMPLATE("353", channel, msg, client_id);
}

void Context::RPL_ENDOFNAMES(int client_id, Channel &channel)
{
	RPL_TEMPLATE("366", channel, "End of /NAMES list", client_id);
}