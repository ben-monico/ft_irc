/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 23:58:54 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/19 19:41:42 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

void err_out(std::string str)
{
	std::cout << str << std::endl;
	exit(1);
}

// int main(int ac, char **av)
// {
// 	// if (ac != 3)
// 	// 	err_out("Wrong number of arguments");
// 	(void) ac;
// 	(void) av;
// 	struct addrinfo hints, *servinfo, *aux;
// 	struct sigaction sa;
// 	int sockfd, new_fd, yes = 1;

// 	ft_bzero(&hints, sizeof(hints));
// 	hints.ai_family = AF_INET;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_flags = AI_PASSIVE; //for my IP

// 	int ret;
// 	std::string err = "Server error: getaddrinfo() - ";
// 	if ((ret = getaddrinfo(NULL, PORT, &hints, &servinfo)))
// 	{
// 		err += gai_strerror(ret);
// 		err_out(err);
// 	}
// 	for (aux = servinfo; aux != NULL; aux = aux->ai_next)
// 	{
// 		sockfd = socket(aux->ai_family, aux->ai_socktype, aux->ai_protocol);
// 		if (sockfd == -1)
// 		{
// 			std::cerr << "Server error: socket()." << std::endl;
// 			continue ;
// 		}
// 		if (setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(yes)) == -1)
// 			err_out("Server error: setsockopt()");
// 		if (bind(sockfd, aux->ai_addr, aux->ai_addrlen) == -1)
// 		{
// 			close(sockfd);
// 			std::cerr << "Server error: bind()." << std::endl;
// 			continue ;
// 		}
// 		break ;
// 	}
// 	freeaddrinfo(servinfo);
// 	if (!aux)
// 		err_out("Server error: unable to bind.");
// 	if (listen(sockfd, BACKLOG) == -1)
// 		err_out("Server error: listen()");
// 	sigemptyset(&sa.sa_mask);
// 	sa.sa_flags = SA_RESTART;
// 	if (sigaction(SIGCHLD, &sa, NULL) == -1)
// 		err_out("Server error: sigaction()");
// 	std::cout << "Waiting for connections..." << std::endl;
// 	socklen_t sin_size;
// 	struct sockaddr_storage user_addr;
// 	char s[INET_ADDRSTRLEN];
// 	while (1)
// 	{
// 		sin_size = sizeof(user_addr);
// 		new_fd = accept(sockfd, (struct sockaddr *)&user_addr, &sin_size);
// 		if (new_fd == -1)
// 		{
// 			std::cerr << "Server error: accept()." << std::endl;
// 			continue ;
// 		}
// 		inet_ntop(user_addr.ss_family, (const void *)AF_INET, s, sizeof(s));
// 		std::cout << "Server: Successful connection from " << s << std::endl;
// 		if (send(new_fd, "what up?", 9, 0) == -1)
// 			std::cerr << "Server error: send()" << std::endl;
// 	}
// }

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	(void)s;
	int saved_errno = errno;
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
	errno = saved_errno;
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
int main(void)
{
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	ft_bzero(&hints, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			err_out("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1)
		{
			err_out("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			err_out("server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo); // all done with this structure
	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	if (listen(sockfd, BACKLOG) == -1)
	{
		err_out("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		err_out("sigaction");
		exit(1);
	}
	printf("server: waiting for connections...\n");
	while (1)
	{ // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			err_out("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family,
				  get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);
		if (!fork())
		{				   // this is the child process
			close(sockfd); // child doesn't need the listener
			if (send(new_fd, "Hello, world!", 13, 0) == -1)
				err_out("send");
			close(new_fd);
			exit(0);
		}
		close(new_fd); // parent doesn't need this
	}
	return 0;
}