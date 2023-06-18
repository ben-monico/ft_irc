#include <Handler.hpp>
#include <climits>

Handler::Handler( std::string port, std::string password): _fdsCount(0), _fdsSize(8), _port(port), _password(password)
{
	_init();

}
Handler::~Handler()
{
	if (_pollFDsArray)
		delete [] _pollFDsArray;
}

void	Handler::start( void )
{
	if (_socketFD == -1)
		exit(pError("_socketFDs", "no bound socket fds, please init", 9));
	// printHostname();
	listenBoundSocket();
	handleClientServerConnections();
}

void	Handler::closeConection( int position )
{
	delFromFDsArray(position);
}

void	Handler::sendAllBytes(std::string const &msg, int clientId)
{
	int bytesSent;

	std::cout << ">> Sending response to fd in position " << clientId << ":\n" << msg << "#====================#" << std::endl;
	while ((bytesSent = send(_pollFDsArray[clientId].fd, msg.c_str(), strlen(msg.c_str()), 0)) != (int)msg.size())
	{
		if (bytesSent == -1)
			return ((void)pError("send error", "error sending bytes to clients", 1));	
		bytesSent += send(_pollFDsArray[clientId].fd, &msg[bytesSent], strlen(msg.c_str()) - bytesSent, 0);
	}
}

void	Handler::setPollCountToEnd( void )
{
	_pollCount = INT_MIN;
}