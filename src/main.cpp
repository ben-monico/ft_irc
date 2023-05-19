/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 23:58:54 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/19 17:47:52 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

void err_out(std::string str)
{
	std::cout << str << std::endl;
	exit(1);
}

int main(int ac, char **av)
{
	if (ac != 3)
		err_out("Wrong number of arguments");
	struct addrinfo hints, *servinfo, *aux;
	struct sockaddr_storage user_addr;
	socklen_t sin_size;
	struct sigaction sa;
	char s[INET_ADDRSTRLEN];
	int sockfd, new_fd, yes = 1;

	ft_bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; //for my IP

	int ret;
	std::string err = "Server error: getaddrinfo() - ";
	if (ret = getaddrinfo(NULL, PORT, &hints, &servinfo))
	{
		err += gai_strerror(ret);
		err_out(err);
	}
	for (aux = servinfo; aux != NULL; aux->ai_next)
	{
		sockfd = socket(aux->ai_family, aux->ai_socktype, aux->ai_protocol);
		if (sockfd == -1)
		{
			std::cerr << "Server error: socket()." << std::endl;
			continue ;
		}
		if (setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(yes)) == -1)
			err_out("Server error: setsockopt()");
		if (bind(sockfd, aux->ai_addr, aux->ai_addrlen) == -1)
		{
			close(sockfd);
			std::cerr << "Server error: bind().";
			continue ;
		}
		break ;
	}
	freeaddrinfo(servinfo);
	if (!aux)
		err_out("Server error: unable to bind.");

}