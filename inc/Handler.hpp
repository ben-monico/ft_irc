#ifndef HANDLER_HPP
# define HANDLER_HPP
# define MYPORT "30035" // the port users will be connecting to
# define BACKLOG 10 // how many pending connections queue will hold

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
#include <ircserv.hpp>
#include <map>

//Static backend handler
class Handler 
{
	private:
		void	handleClientServerConnections();
		void	handlePollResults(int &fdsSize, int &fdsCount);
		void	handleClientConnection(int &fdsSize, int &fdsCount, int position);
		void	parseNewClientInfo(std::string const &str, int id);
		void	acceptIncomingConnection(int &fdsSize, int &fdsCount);
		void	delFromFDsArray(int &fdsCount, int &fdsSize, int position);
		void	addToFDsArray(int &fdsCount, int &fdsSize, int newFD);
		void	extendFDsArray(int &fdsSize);
		void	listenBoundSocket();
		void	printHostname();
		void	parseResponse(std::string buf, int position);
		addrinfo	*getServerInfo();
		std::vector<int>	bindSocketFDs(struct addrinfo *servinfo);
		std::vector<int>	_socketFDs;
		struct pollfd		*_pollFDsArray;

	public:
		int		pError(std::string category, std::string error, int code);
		void	start( void );
		void	sendAllBytes(std::string msg, int clientId);
		void	init( void );
		Handler() { }
		~Handler() { }
	
};

#endif