#include <Channel.hpp>
#include <algorithm>
#include <Handler.hpp>
#include <Context.hpp>
#include <sstream>

Channel::Channel(std::string name)
{
	_name = name;
	_topic = "";
	_userLimit = 0;
	_userCount = 0;
	_chanOpCount = 0;
	_inviteOnly = false;
	_topicOpOnly = false;
	_key = "";
}

Channel::~Channel() {}

//Getters
std::string Channel::getName() const { return _name; }

std::string Channel::getTopic() const { return _topic; }

int Channel::getUserLimit() const { return _userLimit; }

bool Channel::getInviteOnly() const { return _inviteOnly; }

bool Channel::getTopicOpOnly() const { return _topicOpOnly; }

std::string Channel::getKey() const { return _key; }

int Channel::getUserCount() const { return _userCount; }

bool Channel::isFull() const { return (_userLimit != 0 && _userCount >= _userLimit); }

std::map<int, std::string> Channel::getUsersIn() { return _usersID; }

bool Channel::isUserInChannel(int userID) 
{
	std::map<int, std::string>::iterator it = _usersID.begin();

	for ( ; it != _usersID.end() && it->first != userID; ++it) {}
	return (it != _usersID.end());
}

void Channel::getModes(int client_id, Handler *server, std::string const &host) const
{
	std::stringstream ss;
	ss << _userLimit;
	std::string offMode = "-", onMode = "+";

	_inviteOnly ? onMode += "i" : offMode += "i";
	_topicOpOnly ? onMode += "t" : offMode += "t";
	_userLimit	? onMode += "l" : offMode += "l";
	!_key.empty() ? onMode += "k" : offMode += "k";

	if (onMode.size() > 1)
		server->sendAllBytes(host + "324 " + Context::findClientByID(client_id)->getNick() + " #" + _name + " " + onMode + "\r\n", client_id);
	else 
		server->sendAllBytes(host + "324 " + Context::findClientByID(client_id)->getNick() + " #" + _name + "\r\n", client_id);
}

//Setters
void Channel::setTopic(std::string const &topic) { _topic = topic; }

void Channel::setUserLimit(int limit) { _userLimit = limit; }

void Channel::setKey(std::string const &key) { _key = key; }

void Channel::toggleRestrictTopic() { _topicOpOnly = _topicOpOnly ? false : true; }

void Channel::toggleInviteOnly() { _inviteOnly = _inviteOnly ? false : true; }

//Channel functions
void Channel::addClient(int id, const std::string &mode)
{
	bool isUserIn = (_usersID.find(id) != _usersID.end());
	if (!isUserIn)
		_userCount++;
	if (mode == "@" && (!isUserIn || _usersID[id] == "+"))
		_chanOpCount++;
	_usersID[id] = mode;
}

void Channel::changeClientMode(int id, const std::string &mode)
{
	std::map<int, std::string>::iterator it = _usersID.find(id);
	if (it != _usersID.end())
	{
		if (it->second == "+" && mode == "@")
			_chanOpCount++;
		else if (it->second == "@" && mode == "+")
		{
			_chanOpCount--;
			_chanOpCount == 0 ? Context::autoOp(*this) : (void)0;
		}
		_usersID[id] = mode;
	}
}

void Channel::removeClient(int id)
{
	std::map<int, std::string>::iterator it = _usersID.find(id);
	if (it != _usersID.end())
	{
		std::string mode = it->second;
		_usersID.erase(it);
		_userCount--;
		if (mode == "@")
		{
			_chanOpCount--;
			_chanOpCount == 0 ? Context::autoOp(*this) :  (void)0;
		}
	}
}

void Channel::broadcastMsg(std::string const &msg, Handler *server, int senderID)
{
	std::map<int, std::string>::iterator it = _usersID.begin();

	for (; it != _usersID.end(); it++)
	{
		if (it->first != senderID)
			server->sendAllBytes(msg, it->first);
	}
}

void Channel::replaceClientID(int old_id, int new_id)
{
	std::map<int, std::string>::iterator it = _usersID.find(old_id);
	if (it != _usersID.end())
	{
		std::string mode = it->second;
		_usersID.erase(it);
		_usersID[new_id] = mode;
	}
}
