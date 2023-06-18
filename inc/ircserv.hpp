#ifndef IRCSERV_HPP
# define IRCSERV_HPP

# include <iostream>
# include <string>


void	ft_bzero(void *s, size_t n);
size_t	ft_strlen(char *str);
bool	isNumeric(const std::string &str);
bool	isNickValid(const std::string &nick);
void	clean( void );
void	handlesig( int sig );
#endif

