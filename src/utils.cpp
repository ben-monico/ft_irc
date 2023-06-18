#include <ircserv.hpp>
#include <Context.hpp>
#include <Handler.hpp>
#include <Channel.hpp>

void ft_bzero(void *s, size_t n)
{
	char	*p = (char *)s;
	for (size_t i = 0; i < n; i++)
		p[i] = 0;
}

size_t ft_strlen(char *str)
{
	int i = -1;

	if (!str || !*str)
		return (0);
	while (str[++i]) {}
	return (i);
}

bool	isNumeric(const std::string &str)
{
	return (str.find_first_not_of("0123456789") == std::string::npos);
}

bool	isNickValid(const std::string &nick)
{
	return (nick.find_first_not_of("0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM-_") == std::string::npos);
}

void	clean()
{
	Context::getClients().clear();
	Context::getChannels().clear();
}

void	handlesig( int sig )
{
	Handler *server = Context::getServerPtr();

	(void)sig;
	std::cout << std::endl;
	server->setPollCountToEnd();
}