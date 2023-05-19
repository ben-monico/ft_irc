/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 00:00:24 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/19 17:45:53 by bcarreir         ###   ########.fr       */
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

# define PORT "6768"
# define BACKLOG 10


void err_out(std::string str);
void ft_bzero(void *s, size_t n);

#endif
