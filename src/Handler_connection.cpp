#include <Handler.hpp>
#include <Context.hpp>
#include <ircserv.hpp>
#include <Client.hpp>
#include <sstream>
#include <climits>

void	Handler::acceptIncomingConnection()
{
	struct sockaddr_storage remoteaddr;
	socklen_t	addrlen = sizeof(remoteaddr);
	int			newFD;
	
	newFD = accept(_pollFDsArray[0].fd, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newFD < 0)
		pError("accept error", "failed to accept request on fd", 1);
	else
	{
		Context::add_client(_fdsCount);
		addToFDsArray(newFD);
	}
}

int	Handler::buildResponse(std::string buf, int position)
{
	std::vector<Client>::iterator	user = Context::findClientByID((position));
	std::string						cmd;	
	std::istringstream				ss(buf);

	if (!Context::isClientInVector(user))
		exit (pError("parseResponse", "user not found", position));
	std::vector<std::string>	&cmds = user->getCmds();
	if (cmds.size() && cmds.back().find("\r") != std::string::npos)
		cmds.erase(user->getCmds().begin(), user->getCmds().end());
	while (!ss.eof())
	{
		std::getline(ss, cmd, '\n');
		if (cmd.size())
			cmds.push_back(cmd);
	}
	return (cmds.size() && cmds.back().find("\r") != std::string::npos);
}

void	Handler::getLoginInfo(std::string buf, int position)
{
	std::vector<Client>::iterator	user = Context::findClientByID((position));
	std::string						cmd;
	std::istringstream				ss(buf);

	if (!Context::isClientInVector(user))
		exit (pError("parseResponse", "user not found", position));
	std::vector<std::string>		&cmds = user->getCmds();
	while (!ss.eof())
	{
		std::getline(ss, cmd, '\n');
		if (cmd.length())
			cmds.push_back(cmd);
	}
	if (Context::loginInfoFound(user))
		Context::verifyLoginInfo(position);
}

void	Handler::handleClientConnection(int position)
{
	char	buf[513];
	ft_bzero(buf, sizeof(buf));
	int		bytesReceiveded = recv(_pollFDsArray[position].fd, buf, 512, 0);

	std::cout << ">> Receiving data from fd in position " << position << ":\n"  << buf << "#====================#" << std::endl;
	if (bytesReceiveded <= 0)
	{
		delFromFDsArray(position);
		return ;
	}
	if (!Context::findClientByID((position))->getInit())
		getLoginInfo(buf, position);
	else if (buildResponse(buf, position))
		Context::execClientCmds((position));
}

void	Handler::handleClientServerConnections()
{
	addToFDsArray(_socketFD);
	while (1)
	{
		_pollCount = poll(_pollFDsArray, _fdsCount, -1);
		if (_pollCount < 0)
			cleanServerData();
		else if (_pollCount)
			handlePollResults();
	}
	if (_pollFDsArray)
	{
		delete [] _pollFDsArray;
		_pollFDsArray = 0;
	}
}

