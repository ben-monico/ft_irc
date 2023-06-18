#include <Context.hpp>
#include <Channel.hpp>
#include <Handler.hpp>
#include <sstream>
#include <Bot.hpp>

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
		std::cout << *it << " in info f" << std::endl;
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


std::vector<Channel>::iterator	Context::findChannelByName(std::string name)
{
	std::vector<Channel>::iterator it = _channels.begin();
	for (; it != _channels.end(); ++it)
		if (it->getName() == name)
			break;
	return it;	
}

std::vector<Client>::iterator Context::findClientByID(int id)
{
	std::vector<Client>::iterator it = _clients.begin();
	for (; it != _clients.end(); ++it)
		if (it->getID() == id)
			break;
	return it;	
}

std::vector<Client>::iterator  Context::find_client_by_nick(const std::string &nick)
{
	std::vector<Client>::iterator it = _clients.begin();
	for (; it != _clients.end(); ++it)
		if (it->getNick() == nick)
			break;
	return it;	
}

std::vector<Client>::iterator 	Context::find_client_by_username(std::string const& username)
{
	std::vector<Client>::iterator it = _clients.begin();
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
	std::vector<Client>::iterator clientToErase = Context::findClientByID(id_erase);
	if (isClientInVector(clientToErase))
	{
		removeClientFromAllChannels(clientToErase);
		_clients.erase(clientToErase);
		replaceClientID(id_replace, id_erase);
	}
}

void Context::replaceClientID(int old_id, int new_id)
{
	std::vector<Client>::iterator client = Context::findClientByID(old_id);
	if (isClientInVector(client) && old_id != new_id)
	{
		std::map<std::string, std::string> channels = client->getChannels();
		std::map<std::string, std::string>::iterator it = channels.begin();
		for (; it != channels.end(); ++it)
		{
			std::vector<Channel>::iterator channel = findChannelByName(it->first);
			if (isChannelInVector(channel))
				channel->replaceClientID(old_id, new_id);
		}
		client->setID(new_id);
	}
}

void Context::addClientToChannel(std::vector<Client>::iterator const &client, std::vector<Channel>::iterator const &channel, std::string const& mode)
{
	if (isClientInVector(client) && isChannelInVector(channel))
	{
		channel->addClient(client->getID(), mode);
		client->addChannelMode(channel->getName(), mode);
	}
}

void Context::removeClientFromChannel(std::vector<Client>::iterator const &client, std::vector<Channel>::iterator const &channel)
{
	if (isClientInVector(client) && isChannelInVector(channel))
	{
		channel->removeClient(client->getID());
		client->removeChannel(channel->getName());
	}
}

void Context::removeClientFromAllChannels(std::vector<Client>::iterator const & client)
{
	if (isClientInVector(client))
	{ 
		std::map<std::string, std::string> channels = client->getChannels();
		std::map<std::string, std::string>::iterator it = channels.begin();
		for (; it != channels.end(); ++it)
		{
			std::vector<Channel>::iterator channel = findChannelByName(it->first);
			removeClientFromChannel(client, channel);
		}
	}
}

void Context::autoOp(Channel &channel)
{
	std::cout << "tried autoop" << std::endl;
	std::vector<Channel>::iterator channelit = findChannelByName(channel.getName());
	std::map<int, std::string> users = channel.getUsersIn();
	std::map<int, std::string>::iterator it = users.begin();
	for (; it != users.end(); ++it)
	{
		std::vector<Client>::iterator client = findClientByID(it->first);
		if (isClientInVector(client) && client->getChannelMode(channel.getName()) == "+")
		{
			removeClientFromChannel(client, channelit);
			addClientToChannel(client, channelit, "@");
			channel.broadcastMsg(":BOT!BOT@localhost MODE #" + channel.getName() + " +o " + client->getNick() + "\r\n", server, -1);
			break;
		}
	}
}

void	Context::verifyLoginInfo(int id)
{
	std::vector<Client>::iterator		client = findClientByID(id); 
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
	if (user.find('\r') != std::string::npos)
		user = user.substr(0, user.size() - 1);
	std::cout << pass + " " << nick + " " << user << std::endl;
	if (pass.empty() || nick.empty() || user.empty())
	{
		ERR_NEEDMOREPARAMS(id, "Login: missing password, nick, or user");
		server->closeConection(id);
	}
	else if (!server->isPasswordMatch(pass))
		ERR_PASSWDMISMATCH(id, nick);
	else if (isClientInVector(find_client_by_nick(nick)) || isChannelInVector(findChannelByName(nick))
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
	{
		client->init(nick, user);
		RPL_WELCOME(id);
		Bot::welcome(id);
	}
}

bool Context::isClientInVector(std::vector<Client>::iterator userGot) { return (userGot != _clients.end()); }

bool Context::isChannelInVector(std::vector<Channel>::iterator channelGot) { return (channelGot != _channels.end()); }

std::vector<Client> &Context::getClients() { return _clients; }

std::vector<Channel> &Context::getChannels() { return _channels; }