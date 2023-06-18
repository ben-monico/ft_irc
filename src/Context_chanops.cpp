#include <Context.hpp>
#include <Handler.hpp>
#include <Channel.hpp>
#include <cstdlib>

void Context::chanop_kickUser(int client_id, std::string const &channelName, std::string const &targetName, const std::string &reason)
{
	std::vector<Client>::iterator chanop = findClientByID(client_id);
	std::vector<Client>::iterator target = find_client_by_nick(targetName);
	std::vector<Channel>::iterator channel = findChannelByName(channelName);

	if (!isChannelInVector(findChannelByName(channelName)))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else if (chanop->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else if (!isClientInVector(target))
		ERR_NOSUCHNICK(client_id, targetName);
	else if (!target->isInChannel(channelName))
		ERR_NOTONCHANNEL(client_id, channelName);
	else
	{
		channel->broadcastMsg(":" + chanop->getNick() + " KICK #" + channelName + " " + targetName +
			 " :" + reason + "\r\n", server, -1);
		removeClientFromChannel(target, channel);
	}
}

void Context::chanop_inviteUser(int client_id, std::string const &channel, std::string const &targetName)
{
	std::vector<Client>::iterator chanop = findClientByID(client_id);
	std::vector<Client>::iterator target = find_client_by_nick(targetName);

	if (!isChannelInVector(findChannelByName(channel)))
		ERR_NOSUCHCHANNEL(client_id, channel);
	else if (chanop->getChannelMode(channel) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channel);
	else if (!isClientInVector(target))
		ERR_NOSUCHNICK(client_id, targetName);
	else if (target->isInChannel(channel))
		ERR_USERONCHANNEL(client_id, targetName, channel);
	else
	{
		target->addChannelInvite(channel);
		server->sendAllBytes(_hostname + "341 " + chanop->getNick() + " " + targetName + " #" + channel + " :Inviting " + \
		targetName + " to join #" + channel + "\r\n", chanop->getID());
		server->sendAllBytes(":" + chanop->getNick() + "!" + chanop->getUserName() + "@localhost NOTICE " + \
		targetName + "You have been invited to join #" + channel + " by " + chanop->getNick() + "\r\n", target->getID());
	}
}

void Context::chanop_topic(int client_id, std::string const &channelName, std::string const &newtopic)
{
	std::vector<Channel>::iterator channel = findChannelByName(channelName);
	std::vector<Client>::iterator client = findClientByID(client_id);
	if (!isChannelInVector(channel))
		return ERR_NOSUCHCHANNEL(client_id, channelName);
	else if (channel->getTopicOpOnly() && client->getChannelMode(channelName) != "@")
		return ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else
	{
		channel->setTopic(newtopic);
		channel->broadcastMsg(":" + client->getNick() + " TOPIC #" + channelName + " :" + newtopic + "\r\n", server, -1);
	}
}

// MODES
void Context::chanop_toggleInviteOnly(int client_id, std::string channelName, bool toggle)
{
	std::vector<Channel>::iterator channel = findChannelByName(channelName);
	std::vector<Client>::iterator client = findClientByID(client_id);
	if (!isChannelInVector(channel))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else
	{
		channel->getInviteOnly() == toggle ? (void)0 : channel->toggleInviteOnly();
		channel->getInviteOnly()	\
		? channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " +i\r\n", server, -1)
		: channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " -i\r\n", server, -1);
	}
}

void Context::chanop_toggleTopicRestriction(int client_id, std::string const& channelName, bool toggle)
{
	std::vector<Channel>::iterator channel = findChannelByName(channelName);
	std::vector<Client>::iterator client = findClientByID(client_id);
	if (!isChannelInVector(channel))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else 
	{
		channel->getTopicOpOnly() == toggle ? (void)0 : channel->toggleRestrictTopic();
		channel->getTopicOpOnly()	\
		? channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " +t\r\n", server, -1)
		: channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " -t\r\n", server, -1);	
	}
}

void Context::chanop_key(int client_id, std::string const &channelName, std::string const &key)
{
	std::vector<Channel>::iterator channel = findChannelByName(channelName);
	std::vector<Client>::iterator client = findClientByID(client_id);
	if (!isChannelInVector(channel))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else
	{
		key.empty()	? channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " -k " + key + "\r\n", server, -1)
					: channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " +k " + key + "\r\n", server, -1);
		channel->setKey(key);
	}
}

void Context::chanop_toggleOpPriv(int client_id, std::string const& channelName, std::string const& targetNick, std::string toggle) 
{
	std::vector<Client>::iterator	target = find_client_by_nick(targetNick);
	std::vector<Client>::iterator	client = findClientByID(client_id);
	std::vector<Channel>::iterator	channel = findChannelByName(channelName);
	if (!isChannelInVector(channel))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else if (!isClientInVector(target))
		ERR_NOSUCHNICK(client_id, targetNick);
	else if (!target->isInChannel(channelName))
		ERR_NOTONCHANNEL(client_id, channelName);
	else if (client->getNick() == targetNick)
		return ;
	else
	{
		target->getChannelMode(channelName) == toggle ? (void)0 : target->addChannelMode(channelName, toggle);
		target->getChannelMode(channelName) == "@"
		? channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " +o " + targetNick + "\r\n", server, -1)
		: channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " -o " + targetNick + "\r\n", server, -1);
		target->getChannelMode(channelName) == "@" ? channel->changeClientMode(target->getID(), "@") : channel->changeClientMode(target->getID(), "+");
		if (target->getChannelMode(channelName) == "@")
			RPL_WHOREPLY(client_id, *channel);
	}
}

void Context::chanop_userLimit(int client_id, std::string const &channelName, std::string const &userLimit)
{
	std::vector<Channel>::iterator channel = findChannelByName(channelName);
	std::vector<Client>::iterator client = findClientByID(client_id);
	if (!isChannelInVector(channel))
		ERR_NOSUCHCHANNEL(client_id, channelName);
	else if (client->getChannelMode(channelName) != "@")
		ERR_CHANOPRIVSNEEDED(client_id, channelName);
	else
	{
		channel->setUserLimit(atoi(userLimit.c_str()));
		channel->getUserLimit()	? channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " +l " + userLimit + "\r\n", server, -1)
			: channel->broadcastMsg(":" + client->getNick() + " MODE #" + channelName + " -l " + userLimit + "\r\n", server, -1);
	}
}
