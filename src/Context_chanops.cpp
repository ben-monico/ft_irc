#include <Context.hpp>
#include <Handler.hpp>
#include <Channel.hpp>
#include <cstdlib>

void Context::chanop_kick_user(int client_id, std::string const& channel, std::string const& target)
{
	std::vector<Client>::iterator chanop = find_client_by_id(client_id);
	std::vector<Client>::iterator it = find_client_by_nick(target);
	if (it != _clients.end())
	{
		it->removeChannelInvite(channel);
		it->eraseChannel(channel);
		server->sendAllBytes(chanop->getNick() + " KICK #" + channel + " " + target + \
		" :You have been kicked by " + chanop->getNick() + "\r\n", it->getId());
	}
}

void Context::chanop_invite_user(int client_id, std::string const& channel, std::string const& target)
{
	std::vector<Client>::iterator chanop = find_client_by_id(client_id);
	std::vector<Client>::iterator it = find_client_by_nick(target);
	if (it != _clients.end())
	{
		it->addChannelInvite(channel);
		server->sendAllBytes(chanop->getNick() + " INVITE " + target + " #" + channel + "\r\n", it->getId());
	}
}

void Context::chanop_topic(int client_id, std::string const& channel, std::string const& newtopic)
{
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (it != _channels.end())
	{
		newtopic.empty() ? RPL_TOPIC(client_id, *it) : it->setTopic(newtopic);
		server->sendAllBytes(":" + client->getNick() + " TOPIC #" + channel + " :" + newtopic + "\r\n", client_id);
	}
}

//MODES
void Context::chanop_toggle_inviteonly(int client_id, std::string channel)
{
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (it != _channels.end())
	{
		it->toggleInviteOnly();
		server->sendAllBytes(":" + client->getNick() + " MODE #" + channel + " +i\r\n", client->getId());
	}
}

void Context::chanop_toggle_topic_restriction(int client_id, std::string const& channel)
{
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (it != _channels.end())
	{
		it->toggleRestrictTopic();
		server->sendAllBytes(":" + client->getNick() + " MODE #" + channel + " +t\r\n", client->getId());
	}
}

void Context::chanop_key(int client_id, std::string const& channel, std::string const& key)
{
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (it != _channels.end())
	{
		it->setKey(key);
		server->sendAllBytes(":" + client->getNick() + " MODE #" + channel + " +k " + key + "\r\n", client_id);
	}
}

void Context::chanop_oppriv(int client_id, std::string const& channel, std::string const& targetNick) 
{
	std::vector<Client>::iterator it = find_client_by_nick(targetNick);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (it != _clients.end())
		it->addChannelMode(channel, "o");
	server->sendAllBytes(":" + client->getNick() + " MODE #" + channel + " +o " + targetNick + "\r\n", client_id);
}

void Context::chanop_userlimit(int client_id, std::string const& channel, std::string const& userLimit)
{
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (it != _channels.end())
		it->setUserLimit(atoi(userLimit.c_str()));
	server->sendAllBytes(":" + client->getNick() + " MODE #" + channel + " +l " + userLimit + "\r\n", client_id);
}
