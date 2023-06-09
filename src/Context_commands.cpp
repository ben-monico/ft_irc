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


void	Context::execClientCmds(int id)
{
	std::vector<Client>::iterator		client = find_client_by_id(id); 
	std::vector<std::string>			&cmds = client->getCmds();
	std::vector<std::string>::iterator	it = cmds.begin();

	//pass through cmd_parser
	for (; it != cmds.end(); ++it)
	{
		if (it->find("QUIT") != std::string::npos)
		{
			server->closeConection(client->getId());
			break ;
		}
		else if (it->find("JOIN #", 0) != std::string::npos)
		{
			std::string channel = it->substr(6, it->length() - 7);
			cmd_join(client->getId(), channel, "");
		}
		else if (it->find("PRIVMSG ", 0) != std::string::npos)
		{
			//must go with # if channel
			std::string recipient = it->substr(8, it->find(" :", 8) - 8);
			//msg body after :
			std::string msg = it->substr(it->find(" :", 8) + 2, it->length() - it->find(" :", 8) - 3);
			cmd_sendPM(client->getId(), recipient, msg);
		}
		else if (it->find("MODE ", 0) != std::string::npos)
		{
			std::vector<Channel>::iterator channel = find_chan_by_name(it->substr(5, it->length() - 6));
			if (isChannelInVector(channel))
				RPL_CHANNELMODEIS(client->getId(), *channel);
		}
		// else if (it->find("WHO ", 0) != std::string::npos)
		// {
		// 	std::vector<Channel>::iterator channel = find_chan_by_name(it->substr(4, it->length() - 5));
		// 	if (isChannelInVector(channel))
		// 		RPL_WHOREPLY(client->getId(), *channel);
		// 	else
		// 		ERR_NOSUCHCHANNEL(client->getId(), it->substr(4, it->length() - 5));	
		// }

	//INVITE USER CHANNEL	- invite
	//NICK <nick>			- set nick - all sharacters
	//TOPIC <channel> :<topic>, if no topic but : - set topic to "" - if no topic and no : - show topic
	//LIST - show channels
	//KICK <channel> <tarjet> :<reason>
	//JOIN #<CNAME1> #<CNAME2> #<CNAMEn>
	//QUIT :<REASON>
	//PRIVMSG <channel-pub/user-priv> :msg
	//MODE +<FLAG><<PARAMS>> - can or not be space separated - will trim whitespaces - sep by + and if not flag in itkol, error out - if i or t have params error - else execute flag string
	//	if k or l no params, toggle off pass or limit - if o no params call user retarded
	}		
}