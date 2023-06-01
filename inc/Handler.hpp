/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 14:36:26 by bcarreir          #+#    #+#             */
/*   Updated: 2023/06/01 16:47:13 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLER_HPP
# define HANDLER_HPP
# define MYPORT "30035" // the port users will be connecting to
# define BACKLOG 10 // how many pending connections queue will hold
// #include <ircserv.hpp>
# include <Context.hpp>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <cstdlib>
#include "colors.h"
#include <arpa/inet.h>
#include <vector>
#include <iomanip>
#include <poll.h>
#include <cstring>
#include <fcntl.h>
//Static backend handler
class Handler 
{
	private:
		void	handleClientServerConnections();
		void	handlePollResults(int &fdsSize, int &fdsCount);
		void	handleClientConnection(int &fdsSize, int &fdsCount, int position);
		void	acceptIncomingConnection(int &fdsSize, int &fdsCount);
		void	delFromFDsArray(int &fdsCount, int &fdsSize, int position);
		void	addToFDsArray(int &fdsCount, int &fdsSize, int newFD);
		void	extendFDsArray(int &fdsSize);
		void	listenBoundSocket();
		void	printHostname();
		addrinfo	*getServerInfo();
		std::vector<int>	bindSocketFDs(struct addrinfo *servinfo);
		std::vector<int>	_socketFDs;
		struct pollfd		*_pollFDsArray;

	public:
		int		pError(std::string category, std::string error, int code);
		void	start( void );
		void	sendAllBytes(std::string msg, int clientId);
		void	init( void );
	
};

#endif