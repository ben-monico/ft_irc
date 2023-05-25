/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 00:00:24 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/25 23:13:25 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_H
# define FT_IRC_H

# include <iostream>
# include <string>
# include <sys/socket.h>
# include <sys/types.h>
# include <stdlib.h>
# include <netdb.h>
# include <signal.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <netinet/in.h>

# include "Client.hpp"
# include "Channel.hpp"
# include "Context.hpp"


# define PORT "3490"
# define BACKLOG 10
# define MAXDATASIZE 300 

//Utils
void ft_bzero(void *s, size_t n);

void parse_new_client(std::string str);
void parse_recv(std::string str);
bool welcome_client(int sockfd);

void err_out(std::string str);

#endif
