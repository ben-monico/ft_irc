#include <Client.hpp>
#include <Context.hpp>
#include <Channel.hpp>

Client::Client(int id) :_id(id), _init(false)
{
	std::cout << "Connection established with client " << id << std::endl;
}

Client::~Client() { }

//Setters
void Client::setNick(std::string const &nick) { _nick = nick; }

void Client::setUserName(std::string const &userName) { _userName = userName; }

void Client::setID(int	id) { _id = id; }

//Getters
std::string Client::getNick() const { return _nick; }

std::string Client::getUserName() const { return _userName; }

int Client::getID() const { return _id; }

int	Client::getInit() const { return (_init); }

std::vector<std::string> &Client::getCmds() { return (_cmds); }

std::string Client::getChannelMode(std::string const & channel) const 
{
	std::map<std::string, std::string>::const_iterator it = _channelModes.find(channel);
	if (it == _channelModes.end())
		return "-";
	return it->second;
}

std::map<std::string, std::string> Client::getChannels() const
{
	return _channelModes;
}

bool Client::isInChannel(std::string const &channel)
{
	if (_channelModes.find(channel) == _channelModes.end())
		return false;
	return true;
}

bool Client::isInvitedToChannel(std::string const &channel)
{
	std::vector<std::string>::iterator it = _channelInvites.begin();
	for (; it != _channelInvites.end(); it++)
	{
		if (*it == channel)
			return true;
	}
	return false;
}

bool Client::isOnChannel(std::string const &channel) const
{
	std::map<std::string, std::string>::const_iterator it = _channelModes.find(channel);
	if (it == _channelModes.end())
		return false;
	return true;
}

//Client functions
void Client::init(std::string nick, std::string userName)
{ 
	_nick = nick;
	_userName = userName;
	_init = true;
}

void Client::addChannelMode(std::string const &channel, std::string const &mode)
{
	_channelModes[channel] = mode;
}

void Client::addChannelInvite(std::string const &channel)
{
	_channelInvites.push_back(channel);
}

void Client::removeChannelInvite(std::string const &channel)
{
	std::vector<std::string>::iterator it = _channelInvites.begin();
	for (; it != _channelInvites.end(); it++)
	{
		if (*it == channel)
		{
			_channelInvites.erase(it);
			return ;
		}
	}
}

void Client::removeChannel(std::string const &channel)
{
	std::map<std::string, std::string>::iterator it = _channelModes.find(channel);
	if (it != _channelModes.end())
		_channelModes.erase(it);
}
