/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 18:43:31 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/26 14:26:53 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_irc.h>
#include <Context.hpp>


//format NICK Alexa^M$USER Alexandra 0 * :realname^M$
//we can limit usernames and nicknames to alphanumeric + _
void parse_new_client(std::string str, int id)
{
	//USER | ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRE
	std::string nick;
	std::string userName;

	size_t i = 5;
	for (; i < str.size(); i++)
	{
		if (str[i] == '\r' && str[i + 1] == '\n')
		{
			nick = str.substr(5, i - 5);
			break;
		}
	}
	//TODO: find user by nick, if nick in use throw err
	size_t j = i + 7;
	for (; j < str.size(); j++)
	{
		if (str[j] == '\r' && str[i + j] == '\n')
		{
			userName = str.substr(i + 7, j - i - 7);
			break;
		}
	}
	Context::add_client(Client(nick, userName, id));
}

//send welcome, extract their info (only shows up at acceptance)
bool welcome_client(int new_fd)
{
	if (send(new_fd, Context::welcome.c_str(), Context::welcome.size(), 0) == -1)
	{
		std::cerr << "Server error: welcome send()" << std::endl;
		close(new_fd);
		return false;
	}
	char buf[MAXDATASIZE];
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

//TODO: pointer to member function for automated parsing (?)
void parse_recv(std::string str)
{
	Client client("", "", 0);
	if (str.rfind("JOIN #", 0) != std::string::npos)
	{
		if (str[6])
		{	
			std::string channel = str.substr(6);
			Context::cmd_join(client, channel);
		}
		
	}
	std::cout << str << std::endl;
}