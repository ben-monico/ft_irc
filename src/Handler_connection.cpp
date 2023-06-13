/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler_connection.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leferrei <leferrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 16:48:23 by bcarreir          #+#    #+#             */
/*   Updated: 2023/06/13 21:27:15 by leferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Handler.hpp>
#include <Context.hpp>
#include <ircserv.hpp>
#include <Client.hpp>
#include <sstream>

void	Handler::sendAllBytes(std::string const &msg, int clientId)
{
	int bytesSent;

	std::cout << "Sending: " << msg << " - to: " << clientId << std::endl;
	while ((bytesSent = send(_pollFDsArray[clientId].fd, msg.c_str(), strlen(msg.c_str()), 0)) != (int)msg.size())
	{
		if (bytesSent == -1)
			return ((void)pError("send error", "error sending bytes to clients", 1));	
		bytesSent += send(_pollFDsArray[clientId].fd, &msg[bytesSent], strlen(msg.c_str()) - bytesSent, 0);
	}
}

void	Handler::closeConection( int position )
{
	delFromFDsArray(position);
}

void	Handler::acceptIncomingConnection()
{
	struct sockaddr_storage remoteaddr;
	socklen_t	addrlen = sizeof(remoteaddr);
	int			newFD;
	// int			k = fdsCount;
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
	std::vector<Client>::iterator	user = Context::find_client_by_id((position));
	std::string						cmd;	
	std::istringstream				ss(buf);

	if (!Context::isUserInVector(user))
		exit (pError("parseResponse", "user not found", position));
	std::vector<std::string>	&cmds = user->getCmds();
	if (!cmds.empty() && cmds.back().find("\r") != std::string::npos)
		cmds.erase(user->getCmds().begin(), user->getCmds().end());
	while (!ss.eof())
	{
		std::getline(ss, cmd, '\n');
		cmds.push_back(cmd);
	}
	cmds.pop_back();
	for (std::vector<std::string>::iterator it = cmds.begin(); it != cmds.end(); ++it)
		std::cout << *it << std::endl;
	return (cmds.back().find("\r") != std::string::npos);
}

void	Handler::getLoginInfo(std::string buf, int position)
{
	std::vector<Client>::iterator	user = Context::find_client_by_id((position));
	std::string						cmd;
	std::istringstream				ss(buf);

	if (!Context::isUserInVector(user))
		exit (pError("parseResponse", "user not found", position));
	std::vector<std::string>		&cmds = user->getCmds();
	while (!ss.eof())
	{
		std::getline(ss, cmd, '\n');
		if (cmd.length())
			cmds.push_back(cmd);
		std::cout << cmd << std::endl;
	}

	if (Context::loginInfoFound(user))
		Context::verifyLoginInfo(position);
}

void	Handler::handleClientConnection(int position)
{
	char	buf[513];
	ft_bzero(buf, sizeof(buf));
	int		bytesReceiveded = recv(_pollFDsArray[position].fd, buf, 512, 0);

	std::cout << "Receiving msg " << buf << "from " << position << std::endl;
	if (bytesReceiveded <= 0)
	{
		delFromFDsArray(position);
		return ;
	}
	if (!Context::find_client_by_id((position))->getInit())
		getLoginInfo(buf, position);
	else if (buildResponse(buf, position))
		Context::execClientCmds((position));
}

void	Handler::handleClientServerConnections()
{
	int	pollCount;
	
	for (std::vector<int>::iterator it = _socketFDs.begin(); it != _socketFDs.end(); ++it)
		addToFDsArray(*it);
	while (1)
	{
		pollCount = poll(_pollFDsArray, _fdsCount, -1);
		if (pollCount < 0)
		{
			delete [] _pollFDsArray;
			exit(pError("pollcount", "failed to poll", 8));
		}
		else if (pollCount)
			handlePollResults();
	}
	if (_pollFDsArray)
		delete [] _pollFDsArray;
}

