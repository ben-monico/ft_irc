/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler_connection.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 16:48:23 by bcarreir          #+#    #+#             */
/*   Updated: 2023/06/01 17:07:26 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Handler.hpp>
#include <Context.hpp>
#include <ircserv.hpp>
#include <Client.hpp>
#include <Client.hpp>

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

void	Handler::acceptIncomingConnection(int &fdsSize, int &fdsCount)
{
	struct sockaddr_storage remoteaddr;
	socklen_t	addrlen = sizeof(remoteaddr);
	int			newFD;

	newFD = accept(_pollFDsArray[0].fd, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newFD < 0)
		pError("accept error", "failed to accept request on fd", 1);
	else
	{
		Context::add_client(Client(fdsCount));
		addToFDsArray(fdsCount, fdsSize, newFD);
	}
}

void	Handler::handleClientConnection(int &fdsSize, int &fdsCount, int position)
{
	char	buf[384];
	ft_bzero(buf, sizeof(buf));
	int		bytesReceiveded = recv(_pollFDsArray[position].fd, buf, sizeof(buf), 0);

	std::cout << "receiving msg" << std::endl;
	if (bytesReceiveded <= 0)
	{
		std::cout << "Connection closed with " << _pollFDsArray[position].fd << std::endl;
		close(_pollFDsArray[position].fd);
		delFromFDsArray(fdsCount, fdsSize, position);
		return ;
	}
	std::cout << "msg received:\n" << buf << std::endl;
	for (int i = 0; i < fdsCount; ++i)
	{
		if (i == position || !i)
			continue ;
		sendAllBytes(buf, i);
	}
}

void	Handler::handleClientServerConnections()
{
	int	fdsCount = 0, fdsSize = 8, pollCount;
	
	for (std::vector<int>::iterator it = _socketFDs.begin(); it != _socketFDs.end(); ++it)
		addToFDsArray(fdsCount, fdsSize, *it);
	while (1)
	{
		pollCount = poll(_pollFDsArray, fdsCount, -1);
		if (pollCount < 0)
		{
			delete [] _pollFDsArray;
			exit(pError("pollcount", "failed to poll", 8));
		}
		else if (pollCount)
			handlePollResults(fdsSize, fdsCount);
	}
	if (_pollFDsArray)
		delete [] _pollFDsArray;
}