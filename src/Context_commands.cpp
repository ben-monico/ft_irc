#include <Context.hpp>
#include <Channel.hpp>
#include <Client.hpp>
#include <Handler.hpp>

void Context::cmd_join(int client_id, std::string const &channelName, std::string const &key)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	std::vector<Channel>::iterator channel = find_chan_by_name(channelName);

	if (isChannelInVector(channel))
	{
		if (client->isOnChannel(channelName))
			return ERR_USERONCHANNEL(client->getId(), client->getNick(), channelName);
		else
		{
			if (channel->getInviteOnly() && !client->isInvitedToChannel(channelName))
				return ERR_INVITEONLYCHAN(client->getId(), channelName);
			else if (channel->getKey() != key)
				return ERR_BADCHANNELKEY(client->getId(), channelName);
			else if (channel->isFull())
				return ERR_CHANNELISFULL(client->getId(), channelName);
		}
		client->addChannelMode(channelName, "+");
	}
	else
	{
		_channels.push_back(Channel(channelName));
		channel = find_chan_by_name(channelName);
		client->addChannelMode(channelName, "@");
	}
	channel->incrementUserCount(client_id);
	channel->broadcastMsg(":" + client->getNick() + "!" + client->getUserName() + "@localhost JOIN #" +
		channelName + "\r\n", server, -1);
	RPL_TOPIC(client->getId(), *channel);
	RPL_NAMREPLY(client->getId(), *channel);
	RPL_ENDOFNAMES(client->getId(), *channel);
}

void Context::cmd_setNick(int client_id, std::string const & nick)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	std::vector<Client>::iterator nickClient = find_client_by_nick(nick);
	std::vector<Client>::iterator userClient = find_client_by_username(nick);

	if (!isUserInVector(nickClient) && !isUserInVector(userClient) && nick != client->getUserName())
	{
		client->setNick(nick);
		server->sendAllBytes(_hostname + " NICK " + nick + "\r\n", client->getId());
	}
	else if (!isNickValid(nick))
		ERR_ERRONEUSNICKNAME(client->getId(), nick);
	else
		ERR_NICKNAMEINUSE(client->getId(), nick);
}

void Context::cmd_setUserName(int client_id, std::string const & userName)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	std::vector<Client>::iterator nickClient = find_client_by_nick(userName);
	if (!isUserInVector(nickClient))
		client->setUserName(userName);
}

void Context::cmd_sendPM(int sender_id, std::string recipient, std::string const &msg)
{
	std::vector<Client>::iterator sender = find_client_by_id(sender_id);
	if (recipient[0] == '#')
	{
		recipient.erase(0, 1);
		std::vector<Channel>::iterator recipientChannel = find_chan_by_name(recipient);
		if (!isChannelInVector(recipientChannel))
			return ERR_NOSUCHCHANNEL(sender->getId(), recipient);
		else if (!sender->isOnChannel(recipient))
			return ERR_CANNOTSENDTOCHAN(sender->getId(), recipient);
		else
			recipientChannel->broadcastMsg(":" + sender->getNick() + "!" + sender->getUserName() + "@localhost PRIVMSG #" + \
				recipient + " :" + msg + "\r\n", server, sender_id);
	}
	else
	{
		std::vector<Client>::iterator recipientClient = find_client_by_nick(recipient);
		if (isUserInVector(recipientClient))
			server->sendAllBytes(":" + sender->getNick() + "!" + sender->getUserName() + "@localhost PRIVMSG " \
			+ recipient + " :" + msg + "\r\n", recipientClient->getId());
		else
			return ERR_NOSUCHNICK(sender->getId(), recipient);
	}
}

void Context::cmd_part(int client_id, std::string const & channelName, std::string const & reason)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	std::vector<Channel>::iterator channel = find_chan_by_name(channelName);
	if (isChannelInVector(channel) && client->isInChannel(channelName))
	{
		channel->broadcastMsg(":" + client->getNick() + " PART #" + channelName + " :" + reason + "\r\n", server, -1);
		client->eraseChannel(channelName);
		channel->decrementUserCount(client_id);
	}
}

void	Context::execModeOptions(std::vector<std::string> vec, std::vector<Client>::iterator client, const std::string &chan)
{
	if (vec[2][0] == '+')
	{
		if (vec[2][1] == 'o')
			for (std::vector<std::string>::iterator i = vec.begin() + 3; i != vec.end(); ++i)
				chanop_toggleOpPriv(client->getId(), chan, *i, "@");
		if (vec[2][1] == 'l')
			chanop_userLimit(client->getId(), chan, vec[3]);
		if (vec[2][1] == 'k')
			vec.size() > 3 ? chanop_key(client->getId(), chan, vec[3]) : chanop_key(client->getId(), chan, "");
		if (vec[2].find("i") != std::string::npos)
			chanop_toggleInviteOnly(client->getId(), chan, true);
		if (vec[2].find("t") != std::string::npos)
			chanop_toggleTopicRestriction(client->getId(), chan, true);
		return ;
	}
	if (vec[2][1] == 'o')
		for (std::vector<std::string>::iterator i = vec.begin() + 3; i != vec.end(); ++i)
			chanop_toggleOpPriv(client->getId(), chan, *i, "+");
	if (vec[2].find("i") != std::string::npos)
		chanop_toggleInviteOnly(client->getId(), chan, false);
	if (vec[2].find("t") != std::string::npos)
		chanop_toggleTopicRestriction(client->getId(), chan, false);
	if (vec[2].find("k") != std::string::npos)
		chanop_key(client->getId(), chan, "");
	if (vec[2].find("l") != std::string::npos)
		chanop_userLimit(client->getId(), chan, "0");
}