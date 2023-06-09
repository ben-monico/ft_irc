#include <Channel.hpp>
#include <algorithm>
#include <Handler.hpp>
#include <Context.hpp>
Channel::Channel(std::string name) 
{
	_name = name;
	_topic = "";
	_userLimit = 0;
	_userCount = 0;
	_inviteOnly = false;
	_topicOpOnly = false;
	_key = "";
}

Channel::~Channel() {}

std::string Channel::getName() const { return _name; }

std::string Channel::getTopic() const { return _topic; }

int Channel::getUserLimit() const { return _userLimit; }

bool Channel::getInviteOnly() const { return _inviteOnly; }

bool Channel::getTopicOpOnly() const { return _topicOpOnly; }

std::string Channel::getKey() const { return _key; }

int Channel::getUserCount() const { return _userCount; }

bool Channel::isFull() const { return (_userLimit != 0 && _userCount >= _userLimit); }


void Channel::setTopic(std::string const & topic) { _topic = topic; }

void Channel::setUserLimit(int limit) { _userLimit = limit; }

void Channel::setKey(std::string const & key) { _key = key; }

void Channel::toggleRestrictTopic() { _topicOpOnly = _topicOpOnly ? false : true; }

void Channel::toggleInviteOnly() { _inviteOnly = _inviteOnly ? false : true; }

void Channel::decrementUserCount(int id) 
{
	std::vector<int>::iterator it = std::find(_usersID.begin(), _usersID.end(), id);
	if (it != _usersID.end())
	{
		_usersID.erase(it);
		_userCount--; 
	}
}

void Channel::incrementUserCount(int id)
{
	_userCount++;
	_usersID.push_back(id);
}

void Channel::broadcastMsg(std::string const &msg, Handler *server, int senderID)
{
	std::vector<int>::iterator it = _usersID.begin();
	for (; it != _usersID.end(); it++)
	{
		if (*it != senderID)
			server->sendAllBytes(msg + "\r\n", *it);
	}
}

void Channel::replaceClientID(int old_id, int new_id)
{
	std::vector<int>::iterator it = std::find(_usersID.begin(), _usersID.end(), old_id);
	if (it != _usersID.end())
		*it = new_id;
}