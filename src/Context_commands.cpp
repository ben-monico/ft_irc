#include <Context.hpp>
#include <Channel.hpp>
#include <Client.hpp>
#include <Handler.hpp>

void Context::cmd_join(int client_id, std::string const &channel)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	//check if invite only, or for pwd
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it == _channels.end())
	{
		_channels.push_back(Channel(channel));
		it = find_chan_by_name(channel);
		client->addChannelMode(channel, "o");
		//Maybe all should get this message
		server->sendAllBytes(":" + client->getNick() + " JOIN " + channel + "\r\n", client->getId());
	}
	else
	{
		client->addChannelMode(channel, "");
		server->sendAllBytes(":" + client->getNick() + " JOIN " + channel + "\r\n", client->getId());
	}
	RPL_TOPIC(client->getId(), *it);
	RPL_NAMREPLY(client->getId(), *it);
	RPL_ENDOFNAMES(client->getId(), *it);

	// /ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
	// ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
	// ERR_CHANNELISFULL               ERR_BADCHANMASK
	// ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
	// RPL_TOPIC
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

void Context::cmd_sendPM(int client_id, std::string const & msg)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	//get target in message, send() desired PM
	(void)client;
	(void)msg;
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
			cmd_join(client->getId(), channel);
		}
	}		
}