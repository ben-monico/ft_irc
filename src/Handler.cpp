/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 18:14:28 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/31 19:24:25 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Handler.hpp>
#include <ircserv.hpp>

int	Handler::pError(std::string category, std::string error, int code)
{
	std::cerr << White << "#============================================"
		<< "============================================#" << NC << std::endl;
	std::cerr << LightRed << category << ": " << NC << error << NC << std::endl;
	std::cerr << White << "#============================================"
		<< "============================================#" << NC << std::endl;
	return (code);
}

addrinfo *Handler::getServerInfo()
{
	int				status = 0;
	struct addrinfo	*res, hints;
	//make sure its zeroed
	hints.ai_addr = 0;
	hints.ai_addrlen = 0;
	hints.ai_canonname = 0;
	hints.ai_next = 0;
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me
	if ((status = getaddrinfo(NULL, MYPORT, &hints, &res)))
		exit(pError("getaddrinfo error", gai_strerror(status), status));
	return (res);
}

void	Handler::printHostname()
{
	char	host[256];

	if (gethostname(host, 256) == -1)
		exit (pError("hostname", "failed to get hostname", 5));
	std::cout << White << "#======================================="
		<< "=======================================#" << std::endl;
	std::cout << "I" << LightBlue << " Server info:" << White << std::setw(69) << "I\nI "<< NC;
	std::cout << " Hostname:\t"<< host << std::setw(80 - ft_strlen(host) - 10) << White << "I\nI";
	std::cout << NC << "  Port Number:\t" << MYPORT << std::setw(80 - ft_strlen((char *)MYPORT) - 10);
	std::cout << White << "I\n";
	
	std::cout << White << "#======================================="
		<< "=======================================#" << NC << std::endl;
	std::cout << std::endl << std::endl;
}

std::vector<int>	Handler::bindSocketFDs(struct addrinfo *servinfo)
{
	int	yes = 1;
	std::vector<int> _socketFDs;

	for (struct addrinfo *tmp = servinfo; tmp; tmp = tmp->ai_next)
	{
		int k = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (k < 0)
			continue;
		if (setsockopt(k, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(yes)) < 0)
			exit(pError("setsockopt: ", "failed to free up port", 2));
		if (bind(k, tmp->ai_addr, tmp->ai_addrlen) < 0)
			close(k);
		else
		{
			_socketFDs.push_back(k);
			break ;
		}
	}
	return (_socketFDs);
}

void	Handler::listenBoundSocket()
{	
	std::vector<int>::iterator	sockFD = _socketFDs.begin();
	int						status = -1;

	if (_socketFDs.empty())
		exit(pError("bind_socketFDs", "error binding socket to fd", 7));
	for (; sockFD != _socketFDs.end() && status == -1; ++sockFD)
	{
		status = listen(*sockFD, BACKLOG);
		if (status == -1)
			_socketFDs.erase(sockFD);
	}
	if (status == -1)
		exit (pError("socketerr", "failed to listen on all bound sockets", 4));
}

void	Handler::extendFDsArray(int &fdsSize)
{

	struct pollfd	*tmpArray = new struct pollfd[fdsSize * 2];

	for (int i = 0; i < fdsSize * 2; ++i)
	{
		i < fdsSize ? tmpArray[i].fd = _pollFDsArray[i].fd : tmpArray[i].fd = -1;
		i < fdsSize ? tmpArray[i].revents = _pollFDsArray[i].revents : 0;
		tmpArray[i].events = POLLIN;
	}
	delete [] _pollFDsArray;
	_pollFDsArray = tmpArray;
	fdsSize *= 2;
}

void	Handler::addToFDsArray(int &fdsCount, int &fdsSize, int newFD)
{

	if (!_pollFDsArray)
	{
		_pollFDsArray = new struct pollfd[fdsSize];
		for (int i = 0; i < fdsSize; ++i)
			_pollFDsArray[i].fd = -1;
	}
	if (fdsCount == fdsSize)
		extendFDsArray(fdsSize);
	_pollFDsArray[fdsCount].fd = newFD;
	_pollFDsArray[fdsCount].events = POLLIN;
	fdsCount++;
}

void	Handler::delFromFDsArray(int &fdsCount, int &fdsSize, int position)
{
	if (position > fdsSize)
		return ;
	_pollFDsArray[position] = _pollFDsArray[--fdsCount];
	//set client id to new position no spaguetti forgetti
}

void	Handler::acceptIncomingConnection(int &fdsSize, int &fdsCount)
{
	struct sockaddr_storage remoteaddr;
	socklen_t	addrlen = sizeof(remoteaddr);
	std::string msg;
	int			newFD;

	newFD = accept(_pollFDsArray[0].fd, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newFD < 0)
		pError("accept error", "failed to accept request on fd", 1);
	else
	{
		addToFDsArray(fdsCount, fdsSize, newFD);
		msg = handleClientConnection(fdsSize, fdsCount - 1, fdsCount - 1);
		parseNewClientInfo(msg, fdsCount - 1);
	}
}

void Handler::parseNewClientInfo(std::string const &str, int id)
{
	std::string nick;
	std::string userName;

	int pos = str.find("PASS", 13);
	if (pos != -1)
	{
		if (str.substr(pos + 5, str.size() - pos - 7) != PASSWD)
		{
			std::cout << "464 * :Password incorrect." << std::endl;
			close(_pollFDsArray[id].fd);
			delFromFDsArray(id, id, id);
			return ;
		}
	} 
		return ; // close connection instead 
/* 	size_t i = 5;
	for (; i < str.size(); i++)
	{
		if (str[i] == '\r' && str[i + 1] == '\n')
		{
			nick = str.substr(5, i - 5);
			break;
		}
	}
	//TODO: find user by nick, if nick in use throw err
	size_t j = i + 7;
	for (; j < str.size(); j++)
	{
		if (str[j] == '\r' && str[i + j] == '\n')
		{
			userName = str.substr(i + 7, j - i - 7);
			break;
		}
	} */
	Context::add_client(Client(nick, userName, id));
}

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

//consider changing name to receive qqr coisa
std::string	Handler::handleClientConnection(int &fdsSize, int fdsCount, int position)
{
	char	buf[384];
	ft_bzero(buf, sizeof(buf));
	int		bytesReceiveded = recv(_pollFDsArray[position].fd, buf, sizeof(buf), 0);

	// std::cout << "receiving msg" << std::endl;
	if (bytesReceiveded <= 0)
	{
		std::cout << "Connection closed with " << _pollFDsArray[position].fd << std::endl;
		close(_pollFDsArray[position].fd);
		delFromFDsArray(fdsCount, fdsSize, position);
		return "";
	}
	std::cout << "msg received:\n" << buf << std::endl;
	for (int i = 0; i < fdsCount; ++i)
	{
		if (i == position || !i)
			continue ;
		sendAllBytes(buf, i);
	}
	return buf;
}

void	Handler::handlePollResults(int &fdsSize, int &fdsCount)
{
	for (int i = 0; i < fdsCount; ++i)
	{
		if (!_pollFDsArray[i].revents)
			continue ;
		if (!i) //is self aka listener fd
			acceptIncomingConnection(fdsSize, fdsCount);
		else
			handleClientConnection(fdsSize, fdsCount, i);
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

void	Handler::init( void )
{
	struct addrinfo		*servinfo;
	servinfo = getServerInfo();
	_pollFDsArray = 0;
	_socketFDs = bindSocketFDs(servinfo);
	freeaddrinfo(servinfo);
}

void	Handler::start( void )
{
	if (_socketFDs.empty())
		exit(pError("_socketFDs", "no bound socket fds, please init", 9));
	printHostname();
	listenBoundSocket();
	handleClientServerConnections();
}