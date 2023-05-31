/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 18:43:31 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/31 18:49:50 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ircserv.hpp>
#include <Context.hpp>
#include <Handler.hpp>

//format NICK Alexa^M$USER Alexandra 0 * :realname^M$
//we can limit usernames and nicknames to alphanumeric + _


//send welcome, extract their info (only shows up at acceptance)
/* bool welcome_client(int new_fd)
{
	if (send(new_fd, Context::welcome.c_str(), Context::welcome.size(), 0) == -1)
	{
		std::cerr << "Server error: welcome send()" << std::endl;
		close(new_fd);
		return false;
	}
	char buf[300];
	int recv_bytes = recv(new_fd, buf, sizeof(buf), 0);
	if (recv_bytes <= 0)
	{
		std::cerr << "Server error: welcome recv()." << std::endl;
		close(new_fd);
		return false;
	}
	parse_new_client(buf, new_fd);
	return true;
}

//TODO: pointer to member function for automated parsing (?) \r\n
void parse_recv(std::string str)
{
	//placeholdeer for join channel
	Client client("", "", 0);
	if (str.compare("JOIN #") < 0)
	{
		if (str[6])
		{	
			std::string channel = str.substr(6);
			Context::cmd_join(client, channel);
		}
		
	}
	std::cout << str << std::endl;
} */