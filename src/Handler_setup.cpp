#include <Handler.hpp>
#include <Context.hpp>

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

void	Handler::init( void )
{
	struct addrinfo		*servinfo;
	servinfo = getServerInfo();
	_pollFDsArray = 0;
	_socketFDs = bindSocketFDs(servinfo);
	freeaddrinfo(servinfo);
	Context::setServerPtr(this);
}

void	Handler::start( void )
{
	if (_socketFDs.empty())
		exit(pError("_socketFDs", "no bound socket fds, please init", 9));
	printHostname();
	listenBoundSocket();
	handleClientServerConnections();
}