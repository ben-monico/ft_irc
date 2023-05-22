/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 23:58:54 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/22 19:26:05 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

void err_out(std::string str)
{
	std::cout << str << std::endl;
	exit(1);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int ac, char **av)
{
	// if (ac != 3)
	// 	err_out("Wrong number of arguments");
	(void) ac;
	(void) av;
	struct addrinfo hints, *servinfo, *aux;
	struct sigaction sa;
	int sockfd, new_fd, yes = 1;

	ft_bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; //for my IP

	int ret;
	std::string err = "Server error: getaddrinfo() - ";
	if ((ret = getaddrinfo("0.0.0.0", PORT, &hints, &servinfo)))
	{
		err += gai_strerror(ret);
		err_out(err);
	}
	for (aux = servinfo; aux != NULL; aux = aux->ai_next)
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
			std::cerr << "Server error: bind()." << std::endl;
			continue ;
		}
		break ;
	}
	freeaddrinfo(servinfo);
	if (!aux)
		err_out("Server error: unable to bind.");
	if (listen(sockfd, BACKLOG) == -1)
		err_out("Server error: listen()");
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
		err_out("Server error: sigaction()");
	// char hostname[256];
    // if (gethostname(hostname, sizeof(hostname)) == 0)
	// 	std::cout << "Server hostname: " << hostname << std::endl;
	std::cout << "Waiting for connections..." << std::endl;
	socklen_t sin_size;
	struct sockaddr_storage user_addr;
	char s[INET_ADDRSTRLEN];
	int sendn;
	while (1)
	{
		sin_size = sizeof(user_addr);
		new_fd = accept(sockfd, (struct sockaddr *)&user_addr, &sin_size);
		if (new_fd == -1)
		{
			std::cout << "Server error: accept()." << std::endl;
			continue;
		}
		inet_ntop(user_addr.ss_family, get_in_addr((struct sockaddr *)&user_addr), s, sizeof(s));
		std::cout << "Server: Successful connection from " << s << std::endl;
		
		//Client not receiving messages
		std::string str;
		std::cout << "Send message: ";
		std::getline(std::cin, str);
		sendn = send(new_fd, str.c_str(), str.size(), 0);
		if (sendn == -1)
		{
			std::cerr << "Server error: send()" << std::endl;
			close(new_fd);
			continue;
		}
		std::cout << "Message Sent. Number of bytes: "<< sendn << std::endl;
	}
}
