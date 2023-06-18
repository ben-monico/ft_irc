#include <Handler.hpp>
#include <ircserv.hpp>
#include <csignal>

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Incorrect arguments" << std::endl;
		return (1);
	}
	signal(SIGINT, handlesig);
	signal(SIGQUIT, handlesig);
	signal(SIGTERM, handlesig);
	Handler	server(argv[1], argv[2]);
	server.start();
}
