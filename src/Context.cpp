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
		//need to go over CAP again
		Context::RPL_WELCOME(client->getId());
	}
}
