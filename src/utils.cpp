
#include <ircserv.hpp>

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

size_t lenToIRCEOL(char *str)
{
	int i = -1;

	if (!str || !*str)
		return (0);
	while (str[++i])
	{
		if (str[i] == '\r' && str[i + 1] == '\n')
			return (i);
	}
	return (i);
}	

