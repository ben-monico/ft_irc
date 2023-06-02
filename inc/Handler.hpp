#ifndef HANDLER_HPP
# define HANDLER_HPP
# define MYPORT "30035" // the port users will be connecting to
# define BACKLOG 10 // how many pending connections queue will hold

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
		void	handlePollResults();
		void	handleClientConnection(int position);
		void	parseNewClientInfo(std::string const &str, int id);
		void	acceptIncomingConnection();
		void	delFromFDsArray(int position);
		void	addToFDsArray(int newFD);
		void	extendFDsArray();
		void	listenBoundSocket();
		void	printHostname();
		void	getLoginInfo(std::string buf, int position);
		int		buildResponse(std::string buf, int position);
		addrinfo	*getServerInfo();
		std::vector<int>	bindSocketFDs(struct addrinfo *servinfo);
		std::vector<int>	_socketFDs;
		struct pollfd		*_pollFDsArray;
		int					_fdsCount;
		int					_fdsSize;
		const std::string	_password;
	public:
		int		pError(std::string category, std::string error, int code);
		void	start( void );
		void	closeConection( int position );
		void	sendAllBytes(std::string msg, int clientId);
		void	init( void );
		bool	isPasswordMatch(std::string pw) const;
		Handler(std::string password): _fdsCount(0), _fdsSize(8), _password(password) { }
		~Handler() { }
	
};

#endif