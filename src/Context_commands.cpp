#include <Context.hpp>
#include <Channel.hpp>
#include <Client.hpp>
#include <Handler.hpp>
#include <Bot.hpp>

void Context::cmd_join(int client_id, std::string const &channelName, std::string const &key)
{
	std::vector<Client>::iterator client = findClientByID(client_id);
	std::vector<Channel>::iterator channel = findChannelByName(channelName);
	bool setPasswdOnCreation = false;
	if (isChannelInVector(channel))
	{
		bool hasKey = (channel->getKey() != "");
		if (client->isOnChannel(channelName))
			return ERR_USERONCHANNEL(client->getID(), client->getNick(), channelName);
		else
		{
			if (channel->getInviteOnly() && !client->isInvitedToChannel(channelName))
				return ERR_INVITEONLYCHAN(client->getID(), channelName);
			else if (channel->getKey() != key)
				return ERR_BADCHANNELKEY(client->getID(), channelName, hasKey);
			else if (channel->isFull())
				return ERR_CHANNELISFULL(client->getID(), channelName);
		}
	}
	else
	{
		_channels.push_back(Channel(channelName));
		channel = findChannelByName(channelName);
		setPasswdOnCreation = (!key.empty()) ?  true : false;
	}
	std::string mode = (channel->getUserCount() == 0) ?  "@" : "+";
	addClientToChannel(client, channel, mode);
	channel->broadcastMsg(":" + client->getNick() + "!" + client->getUserName() + "@localhost JOIN #" \
		+ channelName + "\r\n", server, -1);
	RPL_TOPIC(client->getID(), *channel);
	RPL_NAMREPLY(client->getID(), *channel);
	RPL_ENDOFNAMES(client->getID(), *channel);
	setPasswdOnCreation ? chanop_key(client_id, channelName, key) : (void)0;
}

void Context::cmd_setNick(int client_id, std::string const & nick)
{
	std::vector<Client>::iterator client = findClientByID(client_id);
	std::vector<Client>::iterator nickClient = find_client_by_nick(nick);
	std::vector<Client>::iterator userClient = find_client_by_username(nick);

	if (!isClientInVector(nickClient) && !isClientInVector(userClient))
	{
		// ":<oldnickname> NICK :<newnickname>"
		for (std::vector<Client>::iterator i = _clients.begin(); i != _clients.end(); ++i)
			server->sendAllBytes(":" + client->getNick() + " NICK :" + nick + "\r\n", i->getID());
		client->setNick(nick);
	}
	else if (!isNickValid(nick))
		ERR_ERRONEUSNICKNAME(client->getID(), nick);
	else
		ERR_NICKNAMEINUSE(client->getID(), nick);
}

void Context::cmd_setUserName(int client_id, std::string const & userName)
{
	std::vector<Client>::iterator client = findClientByID(client_id);
	std::vector<Client>::iterator nickClient = find_client_by_nick(userName);
	if (!isClientInVector(nickClient))
		client->setUserName(userName);
}

void Context::cmd_sendPM(int sender_id, std::string recipient, std::string const &msg)
{
	std::vector<Client>::iterator sender = findClientByID(sender_id);
	if (msg[0] == '/' || !recipient.compare("BOT"))
		return bot_cmds(sender_id, msg);
	else if (recipient[0] == '#')
	{
		recipient.erase(0, 1);
		std::vector<Channel>::iterator recipientChannel = findChannelByName(recipient);
		if (!isChannelInVector(recipientChannel))
			return ERR_NOSUCHCHANNEL(sender->getID(), recipient);
		else if (!sender->isOnChannel(recipient))
			return ERR_CANNOTSENDTOCHAN(sender->getID(), recipient);
		else
			recipientChannel->broadcastMsg(":" + sender->getNick() + "!" + sender->getUserName() + "@localhost PRIVMSG #" \
				+ recipient + " :" + msg + "\r\n", server, sender_id);
	}
	else
	{
		std::vector<Client>::iterator recipientClient = find_client_by_nick(recipient);
		if (isClientInVector(recipientClient))
			server->sendAllBytes(":" + sender->getNick() + "!" + sender->getUserName() + "@localhost PRIVMSG " \
				+ recipient + " :" + msg + "\r\n", recipientClient->getID());
		else
			return ERR_NOSUCHNICK(sender->getID(), recipient);
	}
}

void Context::cmd_part(int client_id, std::string const & channelName, std::string const & reason)
{
	std::vector<Client>::iterator client = findClientByID(client_id);
	std::vector<Channel>::iterator channel = findChannelByName(channelName);
	if (isChannelInVector(channel) && client->isInChannel(channelName))
	{
		channel->broadcastMsg(":" + client->getNick() + " PART #" + channelName + " :" + reason + "\r\n", server, -1);
		removeClientFromChannel(client, channel);
	}
}

void Context::cmd_quit(int client_id, std::string const & reason)
{
	std::vector<Client>::iterator client = findClientByID(client_id);
	if (isClientInVector(client))
	{ 
		std::map<std::string, std::string> channels = client->getChannels();
		std::map<std::string, std::string>::iterator it = channels.begin();
		for (; it != channels.end(); ++it)
		{
			std::vector<Channel>::iterator channel = findChannelByName(it->first);
			if (isChannelInVector(channel))
				channel->broadcastMsg(":" + client->getNick() + " PART #" + channel->getName() + " :" + reason \
				+ "\r\n", server, client->getID());
		}
	}
	server->closeConection(client_id);
}

void	Context::execModeOptions(std::vector<std::string> vec, std::vector<Client>::iterator client, const std::string &chan)
{
	if (vec[2][0] == '+')
	{
		if (vec[2][1] == 'o')
			for (std::vector<std::string>::iterator i = vec.begin() + 3; i != vec.end(); ++i)
				chanop_toggleOpPriv(client->getID(), chan, *i, "@");
		if (vec[2][1] == 'l')
			chanop_userLimit(client->getID(), chan, vec[3]);
		if (vec[2][1] == 'k')
			vec.size() > 3 ? chanop_key(client->getID(), chan, vec[3]) : chanop_key(client->getID(), chan, "");
		if (vec[2].find("i") != std::string::npos)
			chanop_toggleInviteOnly(client->getID(), chan, true);
		if (vec[2].find("t") != std::string::npos)
			chanop_toggleTopicRestriction(client->getID(), chan, true);
		return ;
	}
	if (vec[2][1] == 'o')
		for (std::vector<std::string>::iterator i = vec.begin() + 3; i != vec.end(); ++i)
			chanop_toggleOpPriv(client->getID(), chan, *i, "+");
	if (vec[2].find("i") != std::string::npos)
		chanop_toggleInviteOnly(client->getID(), chan, false);
	if (vec[2].find("t") != std::string::npos)
		chanop_toggleTopicRestriction(client->getID(), chan, false);
	if (vec[2].find("k") != std::string::npos)
		chanop_key(client->getID(), chan, "");
	if (vec[2].find("l") != std::string::npos)
		chanop_userLimit(client->getID(), chan, "0");
}

void Context::bot_cmds(int client_id, std::string const & msg)
{
	if (msg.find("/help", 0) != std::string::npos)
		return Bot::help(client_id);
	else if (msg.find("/users", 0) != std::string::npos)
		return Bot::users(client_id);
	else if (msg.find("/channels", 0) != std::string::npos)
		return Bot::channels(client_id);
	else
		return Bot::error(client_id);
}