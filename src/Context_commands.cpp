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
		client->addChannelMode(channelName, "");
	}
	else
	{
		_channels.push_back(Channel(channelName));
		channel = find_chan_by_name(channelName);
		client->addChannelMode(channelName, "@");
	}
	channel->incrementUserCount(client_id);
	server->sendAllBytes(":" + client->getNick() + " JOIN " + channelName + "\r\n", client->getId());
	RPL_TOPIC(client->getId(), *channel);
	RPL_NAMREPLY(client->getId(), *channel);
	RPL_ENDOFNAMES(client->getId(), *channel);
}

void Context::cmd_setNick(int client_id, std::string nick)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	//NICK | ERR_NICKCOLLISION ERR_NONICKNAMEGIVEN ERR_NICKNAMEINUSE ERR_ERRONEUSNICKNAME
	client->setNick(nick);
}

void Context::cmd_setUserName(int client_id, std::string userName)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	client->setUserName(userName);
}

void Context::cmd_sendPM(int client_id, std::string const& recipient, std::string const & msg)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	std::vector<Client>::iterator recipientClient = find_client_by_nick(recipient);
	std::vector<Channel>::iterator recipientChannel = find_chan_by_name(recipient);
	std::cout << "!" << msg << "!" <<std::endl;
	std::cout << "!" << recipient << "!" <<std::endl;
	if (isUserInVector(recipientClient))
	{
		server->sendAllBytes(":" + client->getNick() + " PRIVMSG " + recipient + " :" + msg + "\r\n", client->getId());
	}
	else if (isChannelInVector(recipientChannel))
		recipientChannel->broadcastMsg("PRIVMSG #" + recipient + " :" + msg + "\r\n", server);

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
			std::string recipient = it->substr(8, it->find(" ", 8) - 8);
			std::string msg = it->substr(it->find(" :", 8) + 2, it->length() - it->find(" :", 8) - 3);
			cmd_sendPM(client->getId(), recipient, msg);
		}
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