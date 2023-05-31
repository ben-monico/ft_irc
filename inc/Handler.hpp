/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 14:36:26 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/31 19:17:26 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLER_HPP
# define HANDLER_HPP
# define MYPORT "30035" // the port users will be connecting to
# define BACKLOG 10 // how many pending connections queue will hold
# ifndef PASSWD
#  define PASSWD "123"
# endif
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
		std::string	handleClientConnection(int &fdsSize, int fdsCount, int position);
		void	parseNewClientInfo(std::string const &str, int id);
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