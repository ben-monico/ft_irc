/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 23:58:54 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/25 17:51:56 by bcarreir         ###   ########.fr       */
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

int server_setup()
{
	struct addrinfo initialinfo, *servinfo, *aux;
	
	ft_bzero(&initialinfo, sizeof(initialinfo));
	initialinfo.ai_family = AF_INET; //IPV4
	initialinfo.ai_socktype = SOCK_STREAM; //cause we need TCP/IP
	initialinfo.ai_flags = AI_PASSIVE; //for my IP

	int ret, sockfd, yes = 1;
	std::string err = "Server error: getaddrinfo() - ";
	if ((ret = getaddrinfo(NULL, PORT, &initialinfo, &servinfo)))
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
		//By using SOL_SOCKET, we are saying that we want to use the socket layer 
		//By using SO_REUSEADDR, we can use the same port for different sockets without getting an error
		if (setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(yes)) == -1)
			err_out("Server error: setsockopt()");
		//bind the aux->address to the socket
		if (bind(sockfd, aux->ai_addr, aux->ai_addrlen) == -1)
		{
			close(sockfd);
			std::cerr << "Server error: bind()." << std::endl;
			continue ;
		}
		break ;
	}
	freeaddrinfo(servinfo);
	servinfo = NULL;
	if (!aux) //means we couldnt bind to any address
		err_out("Server error: unable to bind.");
	return sockfd;
}

int main(int ac, char **av)
{
	// if (ac != 3)
	// 	err_out("Wrong number of arguments");
	(void) ac;
	(void) av;
	int sockfd = server_setup();
	if (listen(sockfd, BACKLOG) == -1) // check for incoming connections
		err_out("Server error: listen()");
	char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0)
		std::cout << "Server hostname: " << hostname << std::endl;
	std::cout << "Waiting for connections..." << std::endl;
	
	socklen_t user_addr_size;
	struct sockaddr_storage user_addr;
	char user_ip[INET_ADDRSTRLEN];
	int new_fd;
	while (1) // constantly accepting new users that are in accept() queue
	{
		user_addr_size = sizeof(user_addr);
		new_fd = accept(sockfd, (struct sockaddr *)&user_addr, &user_addr_size);
		if (new_fd == -1)
		{
			std::cerr << "Server error: accept()." << std::endl;
			continue;
		}
		//cant use this below
		inet_ntop(user_addr.ss_family, get_in_addr((struct sockaddr *)&user_addr), user_ip, sizeof(user_ip));
		std::cout << "Server: Successful connection from " << user_ip << std::endl;

		std::string str = "", msg = "" ;
		std::cout << "Send message: ";
		std::getline(std::cin, str);
		// str = ":DESKTOP-LILBEN NOTICE Bean :Chupa\r\n";
		str = msg + str + "\r\n";
		if (send(new_fd, str.c_str(), str.size(), 0) == -1)
		{
			std::cerr << "Server error: send()" << std::endl;
			close(new_fd);
			continue;
		}
		//client sends back info on the connected user
		char buf[MAXDATASIZE];
		ft_bzero(buf, MAXDATASIZE);
		if (recv(new_fd, buf, MAXDATASIZE - 1, 0) == -1)
		{
			std::cerr << "Server error: send()" << std::endl;
			close(new_fd);
			continue;
		}
		std::cout << (char  *)buf << std::endl;
	}
}

