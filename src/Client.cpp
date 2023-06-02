#include <Client.hpp>

void Client::setNick(std::string const &nick) { _nick = nick; }

void Client::setUserName(std::string const &userName) { _userName = userName; }


void Client::setID(int	id) { _id = id; }

void Client::setMode(std::string const &channel, std::string const &mode) { _channelModes[channel] = mode; }

//Getters
std::string Client::getNick() const { return _nick; }

std::string Client::getUserName() const { return _userName; }

std::vector<std::string> Client::getChannels() const { return _channels; }

std::string Client::getChannelMode(std::string const & channel) const 
{
	std::map<std::string, std::string>::const_iterator it = _channelModes.find(channel);
	if (it == _channelModes.end())
		return "";
	return it->second;
}

int Client::getId() const { return _id; }

int	Client::getInit() const { return (_init); }

std::vector<std::string> &Client::getCmds() { return (_cmds); }

void Client::init(std::string nick, std::string userName)
{ 
	_nick = nick;
	_userName = userName;
	_init = true;
}

void Client::addChannel(std::string const &channel) { _channels.push_back(channel); }

void Client::eraseChannel(std::string const &channel)
{
	std::vector<std::string>::iterator it = _channels.begin();
	for (; it != _channels.end(); ++it)
		if (*it == channel)
			break;
	if (it != _channels.end())
		_channels.erase(it);
}