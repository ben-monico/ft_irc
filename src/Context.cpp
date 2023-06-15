#include <Context.hpp>
#include <Channel.hpp>
#include <Handler.hpp>
#include <sstream>

std::vector<Channel> Context::_channels;
std::vector<Client> Context::_clients;
Handler *Context::server = 0;
std::string Context::_hostname = ":localhost ";

void	Context::setServerPtr( Handler *serverPtr )
{
	server = serverPtr;
}

Handler *Context::getServerPtr( void )
{
	return server;
}

bool	Context::loginInfoFound(std::vector<Client>::iterator &client)
{
	typedef std::vector<std::string>::iterator iter;
	std::vector<std::string>	&cmds = client->getCmds();
	joinPartialCmdStrings(cmds);
	unsigned int				i = 0x00000000;

	for (iter it = cmds.begin(); it != cmds.end(); ++it)
	{
		if (!it->compare(0, 5, "USER "))
			i += 0xFF;
		else if (!it->compare(0, 5, "NICK "))
			i += 0xFF00;
		else if (!it->compare(0, 5, "PASS "))
			i += 0xFF0000;
	}
	if (cmds.size() >= 3 && i != 0xFFFFFF)
	{
		ERR_NEEDMOREPARAMS(client->getID(), "Login: need more params");
		server->closeConection(client->getID());
	}
	return (i == 0xFFFFFF);
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
		if (it->getID() == id)
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

std::vector<Client>::iterator 	Context::find_client_by_username(std::string const& username)
{
	std::vector<Client>::iterator it = Context::_clients.begin();
	for (; it != _clients.end(); ++it)
		if (it->getUserName() == username)
			break;
	return it;	
}

void	Context::add_client(int client_id)
{
	_clients.push_back(Client(client_id));
}

void	Context::remove_client(int id_erase, int id_replace)
{
	std::vector<Client>::iterator clientToErase = Context::find_client_by_id(id_erase);
	if (isUserInVector(clientToErase))
	{
		clientToErase->removeFromAllChannels();
		_clients.erase(clientToErase);
		std::vector<Client>::iterator clientToReplace = Context::find_client_by_id(id_replace);
		if (id_erase != id_replace)
		{
			clientToReplace->setID(id_erase);
			clientToReplace->replaceIDInChannels(id_replace, id_erase);
		}
	}
}

void	Context::verifyLoginInfo(int id)
{
	std::vector<Client>::iterator		client = find_client_by_id(id); 
	std::vector<std::string>			&cmds = client->getCmds();
	joinPartialCmdStrings(cmds);
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
		else if (!it->compare(0, 5, "QUIT "))
			server->closeConection(id);
	}
	std::stringstream	username(user);
	if (!username.eof())
		std::getline(username, user, ' ');
	if (pass.empty() || nick.empty() || user.empty())
	{
		ERR_NEEDMOREPARAMS(id, "Login: missing password, nick, or user");
		server->closeConection(id);
	}
	else if (!server->isPasswordMatch(pass))
		ERR_PASSWDMISMATCH(id, nick);
	else if (isUserInVector(find_client_by_nick(nick)) || isChannelInVector(find_chan_by_name(nick))
		|| !isNickValid(nick) || !isNickValid(user) || user == nick)
	{
		if (!isNickValid(nick) || !isNickValid(user))
			ERR_ERRONEUSNICKNAME(id, nick);
		else
			ERR_NICKNAMEINUSE(id, nick);
		it = cmds.begin();
		for ( ; it != cmds.end() && it->compare(0, 5, "NICK "); ++it) {}
		cmds.erase(it);
		// server->closeConection(id);
	}
	else
		client->init(nick, user);
}

bool Context::isUserInVector(std::vector<Client>::iterator userGot) { return (userGot != _clients.end()); }

bool Context::isChannelInVector(std::vector<Channel>::iterator channelGot) { return (channelGot != _channels.end()); }

std::vector<Client> &Context::getClients() { return _clients; }

std::vector<Channel> &Context::getChannels() { return _channels; }