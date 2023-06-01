/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler_connection.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leferrei <leferrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 16:48:23 by bcarreir          #+#    #+#             */
/*   Updated: 2023/06/01 19:29:59 by leferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Handler.hpp>
#include <Context.hpp>
#include <ircserv.hpp>
#include <Client.hpp>
#include <sstream>

void	Handler::sendAllBytes(std::string msg, int clientId)
{
	int bytesSent;
	while ((bytesSent = send(_pollFDsArray[clientId].fd, msg.c_str(), msg.size(), 0)) != (int)msg.size())
	{
		if (bytesSent == -1)
			return ((void)pError("send error", "error sending bytes to clients", 1));	
		bytesSent += send(_pollFDsArray[clientId].fd, &msg[bytesSent], msg.size() - bytesSent, 0);
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
		Context::add_client(Client(_fdsCount));
		addToFDsArray(newFD);
	}
}

int	Handler::parseResponse(std::string buf, int position)
{
	std::vector<Client>::iterator	user = Context::find_client_by_id((position));
	std::vector<std::string>		cmds;
	std::string						cmd;	
	std::istringstream				ss(buf);

	if (!Context::isUserInVector(user))
		exit (pError("parseResponse", "user not found", position));
	cmds = user->getCmds();
	if (!cmds.empty() && cmds.back().find("\r") != std::string::npos)
		cmds.erase(user->getCmds().begin(), user->getCmds().end());
	while (!ss.eof() || cmd.find("\r") != std::string::npos)
	{
		std::getline(ss, cmd, '\n');
		cmds.push_back(cmd);
	}
	for (std::vector<std::string>::iterator it = cmds.begin(); it != cmds.end(); ++it)
		std::cout << *it << std::endl;
	return (cmds.back().find("\r") != std::string::npos);
}

void	Handler::getLoginInfo(std::string buf, int position)
{
	std::vector<Client>::iterator	user = Context::find_client_by_id((position));
	std::vector<std::string>		cmds;
	std::string						cmd;
	std::istringstream				ss(buf);

	if (!Context::isUserInVector(user))
		exit (pError("parseResponse", "user not found", position));
	cmds = user->getCmds();
	std::getline(ss, cmd, '\n');
	if (cmds.size() == 4)
		{ } //verifyLoginInfo()}

	
}

void	Handler::handleClientConnection(int position)
{
	char	buf[384];
	ft_bzero(buf, sizeof(buf));
	int		bytesReceiveded = recv(_pollFDsArray[position].fd, buf, sizeof(buf), 0);

	std::cout << "Receiving msg from " << position << std::endl;
	if (bytesReceiveded <= 0)
	{
		delFromFDsArray(position);
		return ;
	}
	// if (Context::find_client_by_id((position))->getInit())
		if (parseResponse(buf, position)) 
			Context::execClientCmds(*Context::find_client_by_id((position)));
	// else
	// 	getLoginInfo(buf, position);
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