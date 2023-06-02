#include <Context.hpp>
#include <Channel.hpp>
#include <Handler.hpp>
#include <sstream>
//ERRORS ON PAGE 43

std::vector<Channel> Context::_channels;
std::vector<Client> Context::_clients;
std::string Context::welcome = "Welcome to ft_irc\r\n";
Handler *Context::server = 0;
std::string Context::_hostname = ":localhost";

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

std::vector<Client>::iterator  Context::find_client_by_id(int id)
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
void Context::cmd_join(Client &client, std::string const &channel)
{
	//check if invite only, or for pwd
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it == _channels.end())
	{
		_channels.push_back(Channel(channel, 'o'));
		it = (_channels.end())--;
		
	}
	//how to set client mode?
	(*it).addClient(client);
	client.addChannel(channel);
	RPL_TOPIC(client.getId(), *it);
	RPL_NAMREPLY(client.getId(), *it);
	RPL_ENDOFNAMES(client.getId(), *it);

	// /ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
	// ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
	// ERR_CHANNELISFULL               ERR_BADCHANMASK
	// ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
	// RPL_TOPIC
}

void Context::cmd_setNick(Client &client, std::string nick)
{
	//NICK | ERR_NICKCOLLISION ERR_NONICKNAMEGIVEN ERR_NICKNAMEINUSE ERR_ERRONEUSNICKNAME
	client.setNick(nick);
}

void Context::cmd_setUserName(Client &client, std::string userName)
{
	client.setUserName(userName);
}

void Context::cmd_sendPM(Client &client, std::string const & msg)
{
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
void Context::chanop_kick(std::string const & channel, Client &client)
{
	//:kickerNick!~kickerUserName@hostname KICK #channel targetuser :You have been kicked by kickuser: Reason for kick.
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it != _channels.end())
	{
		(*it).removeClientFromChannel(client.getNick());
		client.eraseChannel(channel);
	}
	server->sendAllBytes(client.getNick() + " KICK #" + channel + " " + client.getNick() + \
	 " :You have been kicked by " + client.getNick() + "\r\n", client.getId());
}

void Context::chanop_invite(Client &client, std::string channel)
{
	//if channel is invite only mode, invite must be chanop
	//ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
	//    ERR_NOTONCHANNEL                ERR_USERONCHANNEL
	//    ERR_CHANOPRIVSNEEDED
	//    RPL_INVITING                    RPL_AWAY
	(void)client;
	(void)channel;
}

void Context::chanop_topic(Client &client, std::string topic)
{
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

void	Context::add_client(Client client)
{
	_clients.push_back(client);
}

void	Context::remove_client(int id_erase, int id_replace)
{
	std::iter_swap(Context::find_client_by_id(id_erase), Context::find_client_by_id(id_replace));
	_clients.erase(Context::find_client_by_id(id_erase));
	find_client_by_id(id_replace)->setID(id_erase);
}
//Command responses
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
		client->init(nick, user);
}
