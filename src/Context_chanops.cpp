#include <Context.hpp>
#include <Handler.hpp>
#include <Channel.hpp>

void Context::chanop_kick(std::string const& channel, int client_id)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	//:kickerNick!~kickerUserName@hostname KICK #channel targetuser :You have been kicked by kickuser: Reason for kick.
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it != _channels.end())
		client->eraseChannel(channel);
	server->sendAllBytes(client->getNick() + " KICK #" + channel + " " + client->getNick() + \
	 " :You have been kicked by " + client->getNick() + "\r\n", client->getId());
}

void Context::chanop_invite(int client_id, std::string channel)
{
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it != _channels.end())
		it->toggleInviteOnly();
	server->sendAllBytes(":" + client->getNick() + " MODE #" + channel + " +i\r\n", client->getId());
}

void Context::chanop_topic(int client_id, std::string const& channel)
{
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it != _channels.end())
		it->toggleRestrictTopic();
	server->sendAllBytes(":" + client->getNick() + " MODE #" + channel + " +t\r\n", client->getId());
}

void Context::chanop_userlimit(int client_id, std::string const& channel, std::string const& userLimit)
{
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it != _channels.end())
		it->setUserLimit(userLimit);
	server->sendAllBytes(":" + client->getNick() + " MODE #" + channel + " +l " + userLimit + "\r\n", client_id);
}

void Context::chanop_mode(int client_id, std::string const& channel, std::string const& modes) 
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (client->getChannelMode(channel) == "o")
	{
		(void)channel;
		(void)modes;
		server->sendAllBytes(":" + client->getNick() + " MODE " + channel + " +on " + client->getNick() + "\r\n", client->getId());
	}
}

