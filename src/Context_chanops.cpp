#include <Context.hpp>
#include <Handler.hpp>
#include <Channel.hpp>
#include <cstdlib>

void Context::chanop_kick_user(int client_id, std::string const& channel, std::string const& targetName)
{
	std::vector<Client>::iterator chanop = find_client_by_id(client_id);
	std::vector<Client>::iterator target = find_client_by_nick(targetName);
	if (chanop->getChannelMode(channel) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channel);
	else if (!isUserInVector(target))
		ERR_NOSUCHNICK(client_id, targetName);
	else if (!isChannelInVector(find_chan_by_name(channel)))
		ERR_NOSUCHCHANNEL(client_id, channel);
	else if (!target->isInChannel(channel))
		ERR_NOTONCHANNEL(client_id, channel);
	else
	{
		target->removeChannelInvite(channel);
		target->eraseChannel(channel);
		find_chan_by_name(channel)->decrementUserCount(target->getId());
		server->sendAllBytes(chanop->getNick() + " KICK #" + channel + " " + targetName + \
		" :You have been kicked by " + chanop->getNick() + "\r\n", target->getId());
	}
}

void Context::chanop_invite_user(int client_id, std::string const& channel, std::string const& targetName)
{
	std::vector<Client>::iterator chanop = find_client_by_id(client_id);
	std::vector<Client>::iterator target = find_client_by_nick(targetName);
	if (chanop->getChannelMode(channel) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channel);
	else if (!isUserInVector(target))
		ERR_NOSUCHNICK(client_id, targetName);
	else if (!isChannelInVector(find_chan_by_name(channel)))
		ERR_NOSUCHCHANNEL(client_id, channel);
	else if (target->isInChannel(channel))
		ERR_USERONCHANNEL(client_id, targetName, channel);
	else
	{
		target->addChannelInvite(channel);
		server->sendAllBytes(chanop->getNick() + " INVITE " + targetName + " #" + channel + "\r\n", target->getId());
	}
}

void Context::chanop_topic(int client_id, std::string const& channelName, std::string const& newtopic)
{
	std::vector<Channel>::iterator channel = find_chan_by_name(channelName);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (client->getChannelMode(channelName) != "@")
		return ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else if (!isChannelInVector(channel))
		return ERR_NOSUCHCHANNEL(client_id, channelName);
	else if (channel->isFull())
		return ERR_CHANNELISFULL(client_id, channelName);
	
	else
	{
		newtopic.empty() ? RPL_TOPIC(client_id, *channel) : channel->setTopic(newtopic);
		server->sendAllBytes(":" + client->getNick() + " TOPIC #" + channelName + " :" + newtopic + "\r\n", client_id);
	}
}

//MODES
void Context::chanop_toggle_inviteonly(int client_id, std::string channelName)
{
	std::vector<Channel>::iterator channel = find_chan_by_name(channelName);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else if (!isChannelInVector(channel))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else
	{
		channel->toggleInviteOnly();
		server->sendAllBytes(":" + client->getNick() + " MODE #" + channelName + " +i\r\n", client->getId());
	}
}

void Context::chanop_toggle_topic_restriction(int client_id, std::string const& channelName)
{
	std::vector<Channel>::iterator channel = find_chan_by_name(channelName);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else if (!isChannelInVector(channel))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else
	{
		channel->toggleRestrictTopic();
		server->sendAllBytes(":" + client->getNick() + " MODE #" + channelName + " +t\r\n", client->getId());
	}
}

void Context::chanop_key(int client_id, std::string const& channelName, std::string const& key)
{
	std::vector<Channel>::iterator channel = find_chan_by_name(channelName);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else if (!isChannelInVector(channel))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else
	{
		channel->setKey(key);
		server->sendAllBytes(":" + client->getNick() + " MODE #" + channelName + " +k " + key + "\r\n", client_id);
	}
}

void Context::chanop_oppriv(int client_id, std::string const& channelName, std::string const& targetNick) 
{
	std::vector<Client>::iterator target = find_client_by_nick(targetNick);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else if (!isUserInVector(target))
		ERR_NOSUCHNICK(client_id, targetNick);
	else if (!isChannelInVector(find_chan_by_name(channelName)))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else if (!target->isInChannel(channelName))
		ERR_NOTONCHANNEL(client_id, channelName);
	else
	{
		target->addChannelMode(channelName, "@");
		server->sendAllBytes(":" + client->getNick() + " MODE #" + channelName + " +o " + targetNick + "\r\n", client_id);
	}
}

void Context::chanop_userlimit(int client_id, std::string const& channelName, std::string const& userLimit)
{
	std::vector<Channel>::iterator channel = find_chan_by_name(channelName);
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else if (!isChannelInVector(channel))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else
	{
		channel->setUserLimit(atoi(userLimit.c_str()));
		server->sendAllBytes(":" + client->getNick() + " MODE #" + channelName + " +l " + userLimit + "\r\n", client_id);
	}
}
