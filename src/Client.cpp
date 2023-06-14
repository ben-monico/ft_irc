#include <Client.hpp>
#include <Context.hpp>
#include <Channel.hpp>

void Client::setNick(std::string const &nick) { _nick = nick; }

void Client::setUserName(std::string const &userName) { _userName = userName; }

void Client::setID(int	id) { _id = id; }

void Client::addChannelMode(std::string const &channel, std::string const &mode)
{
	if (getChannelMode(channel) == "@" && mode == "+")
		Context::find_chan_by_name(channel)->decrementOpCount();
	else if (getChannelMode(channel) == "+" && mode == "@")
		Context::find_chan_by_name(channel)->incrementOpCount();
	_channelModes[channel] = mode;
}

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

void Client::init(std::string nick, std::string userName)
{ 
	_nick = nick;
	_userName = userName;
	_init = true;
	std::cout << "Client " << nick << " initialized " << std::endl;
	Context::RPL_WELCOME(_id);

}

void Client::removeChannel(std::string const &channel)
{
	if (Context::isChannelInVector(channel))
		Context::find_chan_by_name(channel)->decrementUserCount(_id);
	std::map<std::string, std::string>::iterator it = _channelModes.find(channel);
	if (it != _channelModes.end())
		_channelModes.erase(it);
}

bool Client::isOnChannel(std::string const &channel) const
{
	std::map<std::string, std::string>::const_iterator it = _channelModes.find(channel);
	if (it == _channelModes.end())
		return false;
	return true;
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

void Client::removeFromAllChannels()
{
	std::map<std::string, std::string>::iterator it = _channelModes.begin();
	for (; it != _channelModes.end(); it++)
	{
		removeChannel(it->first);
	}
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

void Client::replaceIDInChannels(int old_id, int new_id)
{
	std::map<std::string, std::string>::iterator it = _channelModes.begin();
	for (; it != _channelModes.end(); it++)
		Context::find_chan_by_name(it->first)->replaceClientID(old_id, new_id);
}