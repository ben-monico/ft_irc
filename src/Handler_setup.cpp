#include <Handler.hpp>
#include <Context.hpp>
#include <ircserv.hpp>
#include <Bot.hpp>

int	Handler::pError(std::string category, std::string error, int code)
{
	std::cerr << White << "#============================================"
		<< "============================================#" << NC << std::endl;
	std::cerr << LightRed << category << ": " << NC << error << NC << std::endl;
	std::cerr << White << "#============================================"
		<< "============================================#" << NC << std::endl;
	return (code);
}

bool	Handler::isPasswordMatch(std::string pw) const
{
		return (_password.compare(pw) == 0);
}

addrinfo *Handler::getServerInfo()
{
	int				status = 0;
	struct addrinfo	*res, hints;

	ft_bzero(&hints, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_family = AF_INET6; // don't care IPv4 or IPv6
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me
	if ((status = getaddrinfo(NULL, _port.c_str(), &hints, &res)))
		exit(pError("getaddrinfo error", gai_strerror(status), status));
	return (res);
}

void	Handler::printHostname()
{
	char	host[256];

	if (gethostname(host, 256) == -1)
		exit (pError("hostname", "failed to get hostname", 5));
	std::cout << std::endl;
	std::cout << White << "#======================================="
		<< "=======================================#" << std::endl;
	std::cout << "I" << LightBlue << " Server info:" << White << std::setw(69) << "I\nI "<< NC;
	std::cout << " Hostname:\t"<< host << std::setw(80 - ft_strlen(host) - 10) << White << "I\nI";
	std::cout << NC << "  Port Number:\t" << _port << std::setw(80 - ft_strlen((char *)_port.c_str()) - 10);
	std::cout << White << "I\n";
	
	std::cout << White << "#======================================="
		<< "=======================================#" << NC << std::endl;
	std::cout << std::endl;
}

int	Handler::bindSocketFD(struct addrinfo *servinfo)
{
	int	yes = 1;

	for (struct addrinfo *tmp = servinfo; tmp; tmp = tmp->ai_next)
	{
		int k = socket(PF_INET6, tmp->ai_socktype, tmp->ai_protocol);
		if (k < 0)
			continue;
		if (setsockopt(k, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(yes)) < 0)
			exit(pError("setsockopt: ", "failed to free up port", 2));
		if (bind(k, tmp->ai_addr, tmp->ai_addrlen) < 0)
			close(k);
		else
			return (k);
	}
	return (-1);
}

void	Handler::listenBoundSocket()
{
	int						status = -1;

	if (_socketFD == -1)
		exit(pError("bind_socketFDs", "error binding socket to fd", 7));
	status = listen(_socketFD, BACKLOG);
	if (status == -1)
		exit (pError("socketerr", "failed to listen on all bound sockets", 4));
}

void	Handler::_init( void )
{
	struct addrinfo		*servinfo;
	if (!isNumeric(_port) || atoi(_port.c_str()) < 1024
		|| atoi(_port.c_str()) > 0xFFFF || _port.length() > 5)
		exit(pError("porterror", "port number out of range or incorrectly formatted", 1));
	servinfo = getServerInfo();
	_pollFDsArray = 0;
	_socketFD = bindSocketFD(servinfo);
	freeaddrinfo(servinfo);
	Context::setServerPtr(this);
	Bot::setServerPtr(this);
}

void	Handler::start( void )
{
	if (_socketFD == -1)
		exit(pError("_socketFDs", "no bound socket fds, please init", 9));
	printHostname();
	listenBoundSocket();
	handleClientServerConnections();
}