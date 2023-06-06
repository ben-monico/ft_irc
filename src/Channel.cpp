#include <Channel.hpp>

Channel::Channel(std::string name) 
{
	_name = name;
	_topic = "";
	_userLimit = 0;
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


void Channel::setTopic(std::string const & topic) { _topic = topic; }

void Channel::setUserLimit(int limit) { _userLimit = limit; }

void Channel::setKey(std::string const & key) { _key = key; }

void Channel::toggleRestrictTopic() { _topicOpOnly = true ? false : true; }

void Channel::toggleInviteOnly() { _inviteOnly = true ? false : true; }