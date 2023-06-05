#include <Context.hpp>
#include <Channel.hpp>
#include <Handler.hpp>
#include <sstream>
//ERRORS ON PAGE 43

std::vector<Channel> Context::_channels;
std::vector<Client> Context::_clients;
Handler *Context::server = 0;
std::string Context::_hostname = ":localhost ";

void	Context::setServerPtr( Handler *serverPtr )
{
	server = serverPtr;
}

std::vector<Channel>::iterator	Context::find_chan_by_name(std::string name)
{
	std::vector<Channel>::iterator it = Context::_channels.begin();
	for (; it != _channels.end(); ++it)
		if (it->getName() == name)
			break;
	return it;	
}

std::vector<Client>::iterator Context::find_client_by_id(int id)
{
	std::vector<Client>::iterator it = Context::_clients.begin();
	for (; it != _clients.end(); ++it)
		if (it->getId() == id)
			break;
	return it;	
}

std::vector<Client>::iterator  Context::find_client_by_nick(const std::string &nick)
{
	std::vector<Client>::iterator it = Context::_clients.begin();
	for (; it != _clients.end(); ++it)
		if (it->getNick() == nick)
			break;
	return it;	
}

Client	&Context::get_client_by_id(int id)
{
	std::vector<Client>::iterator it = Context::_clients.begin();
	for (; it != _clients.end(); ++it)
		if (it->getId() == id)
			break;
	return *it;	
}

//Commands
void Context::cmd_join(int client_id, std::string const &channel)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	//check if invite only, or for pwd
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it == _channels.end())
	{
		_channels.push_back(Channel(channel, 'o'));
		it = find_chan_by_name(channel);
		client->addChannelMode(channel, "@");
		server->sendAllBytes(":" + client->getNick() + " JOIN " + channel + "\r\n", client->getId());
		server->sendAllBytes(":" + client->getNick() + " MODE " + channel + " +on " + client->getNick() + "\r\n", client->getId());
	}
	else 
		client->addChannelMode(channel, "");
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

//Channel operations
void Context::chanop_kick(std::string const & channel, int client_id)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	//:kickerNick!~kickerUserName@hostname KICK #channel targetuser :You have been kicked by kickuser: Reason for kick.
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it != _channels.end())
	{
		client->eraseChannel(channel);
	}
	server->sendAllBytes(client->getNick() + " KICK #" + channel + " " + client->getNick() + \
	 " :You have been kicked by " + client->getNick() + "\r\n", client->getId());
}

void Context::chanop_invite(int client_id, std::string channel)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	//if channel is invite only mode, invite must be chanop
	//ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
	//    ERR_NOTONCHANNEL                ERR_USERONCHANNEL
	//    ERR_CHANOPRIVSNEEDED
	//    RPL_INVITING                    RPL_AWAY
	(void)client;
	(void)channel;
}

void Context::chanop_topic(int client_id, std::string topic)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	(void)client;
	(void)topic;
	// ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
	//    RPL_NOTOPIC                     RPL_TOPIC
	//    ERR_CHANOPRIVSNEEDED
}


void Context::chanop_mode(Channel &channel, char c, std::string const & msg) 
{
	//check msg in case of additional params
	if (c == 'i' || c == 't' || c == 'k' || c == 'o' || c == 'l')
		channel.setMode(c);
	(void) msg;
}

void	Context::add_client(int client_id)
{
	_clients.push_back(Client(client_id));
}

void	Context::remove_client(int id_erase, int id_replace)
{
	std::iter_swap(Context::find_client_by_id(id_erase), Context::find_client_by_id(id_replace));
	_clients.erase(Context::find_client_by_id(id_erase));
	find_client_by_id(id_replace)->setID(id_erase);
}

void Context::addClientToChannel(int client_id, std::string const & channelname, std::string const &mode)
{
	std::vector<Client>::iterator it = find_client_by_id(client_id);
	if (it  != _clients.end())
		it->addChannelMode(channelname, mode);
}

void Context::removeClientFromChannel(int client_id, std::string const & channelname)
{
	std::vector<Client>::iterator it = find_client_by_id(client_id);
	if (it  != _clients.end())
		it->eraseChannel(channelname);
}

void	Context::execClientCmds(int id)
{
	std::vector<Client>::iterator		client = find_client_by_id(id); 
	std::vector<std::string>			&cmds = client->getCmds();
	std::vector<std::string>::iterator	it = cmds.begin();

	
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

void	Context::verifyLoginInfo(int id)
{
	std::vector<Client>::iterator		client = find_client_by_id(id); 
	std::vector<std::string>			&cmds = client->getCmds();
	std::vector<std::string>::iterator	it = cmds.begin();
	std::string							pass = "", nick = "", user = "";

	for ( ; it != cmds.end(); ++it)
	{
		if (!it->compare(0, 5, "PASS "))
			pass = it->substr(5, it->length() - 6);
		else if (!it->compare(0, 5, "NICK "))
			nick = it->substr(5, it->length() - 6);
		else if (!it->compare(0, 5, "USER "))
			user = it->substr(5, it->length() - 5);
	}
	std::stringstream	username(user);
	if (!username.eof())
		std::getline(username, user, ' ');
	if (pass.empty() || !server->isPasswordMatch(pass))
	{
		ERR_PASSWDMISMATCH(id);
		server->closeConection(id);
	}
	else if (isUserInVector(find_client_by_nick(nick)))
	{
		USR_RPL_TEMPLATE("436", "Nickname is already in use.", nick, id);
		// server->closeConection(id);
	}
	else
	{
		client->init(nick, user);
		Context::RPL_WELCOME(client->getId());
	}
}
