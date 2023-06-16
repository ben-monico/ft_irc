#include <Handler.hpp>
#include <ircserv.hpp>

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Incorrect arguments" << std::endl;
		return (1);
	}
	Handler	server(argv[1]);
	server.init();
	server.start();
}
