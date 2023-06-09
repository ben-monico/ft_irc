#include <Context.hpp>
#include <Channel.hpp>
#include <Client.hpp>
#include <Handler.hpp>

void Context::cmd_join(int client_id, std::string const &channelName, std::string const& key)
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
	channel->broadcastMsg(":" + client->getNick() + "!" + client->getUserName() + "@localhost JOIN #" + channelName, server, -1);
	//these need to be broadcast
	RPL_TOPIC(client->getId(), *channel);
	RPL_NAMREPLY(client->getId(), *channel);
	RPL_ENDOFNAMES(client->getId(), *channel);
}

void Context::cmd_setNick(int client_id, std::string nick)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	client->setNick(nick);
}

void Context::cmd_setUserName(int client_id, std::string userName)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	client->setUserName(userName);
}

void Context::cmd_sendPM(int sender_id, std::string recipient, std::string const & msg)
{
	std::vector<Client>::iterator sender = find_client_by_id(sender_id);
	if (recipient[0] == '#')
	{
		recipient.erase(0, 1);
		std::vector<Channel>::iterator recipientChannel = find_chan_by_name(recipient);
		if (isChannelInVector(recipientChannel))
			recipientChannel->broadcastMsg(":" + sender->getNick() + "!" + sender->getUserName() + "@localhost PRIVMSG #" + \
				recipient + " :" + msg, server, sender_id);
		else
			return ERR_NOSUCHCHANNEL(sender->getId(), recipient);
	}
	else
	{
		std::vector<Client>::iterator recipientClient = find_client_by_nick(recipient);
		if (isUserInVector(recipientClient))
			server->sendAllBytes(":" + sender->getNick() + "!" + sender->getUserName() + "@localhost PRIVMSG " + recipient + " :" + msg + "\r\n", recipientClient->getId());
		else
			return ERR_NOSUCHNICK(sender->getId(), recipient);
	}

	// ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
	//    ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
	//    ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
	//    ERR_NOSUCHNICK
	//    RPL_AWAY
}


void	Context::execModeOptions(std::vector<std::string> vec, std::vector<Client>::iterator client, const std::string &chan)
{
	if (vec[2][0] == '+')
	{
		if (vec[2][1] == 'o')
			for (std::vector<std::string>::iterator i = vec.begin() + 3; i != vec.end(); ++i)
				chanop_toggle_oppriv(client->getId(), chan, *i, "@");
		if (vec[2][1] == 'l')
			chanop_userlimit(client->getId(), chan, vec[3]);
		if (vec[2][1] == 'k')
			chanop_key(client->getId(), chan, vec[3]);
		if (vec[2].find("i") != std::string::npos)
			chanop_toggle_inviteonly(client->getId(), chan, true);
		if (vec[2].find("t") != std::string::npos)
			chanop_toggle_topic_restriction(client->getId(), chan, true);
		return ;
	}
	if (vec[2][1] == 'o')
		for (std::vector<std::string>::iterator i = vec.begin() + 3; i != vec.end(); ++i)
			chanop_toggle_oppriv(client->getId(), chan, *i, "+");
	if (vec[2].find("i") != std::string::npos)
		chanop_toggle_inviteonly(client->getId(), chan, false);
	if (vec[2].find("t") != std::string::npos)
		chanop_toggle_topic_restriction(client->getId(), chan, false);
	if (vec[2].find("k") != std::string::npos)
		chanop_key(client->getId(), chan, "");
	if (vec[2].find("l") != std::string::npos)
		chanop_userlimit(client->getId(), chan, "0");
}