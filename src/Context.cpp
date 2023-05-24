/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/24 19:09:34 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/24 20:23:09 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"

std::list<Channel> Context::_channels;
std::list<Client> Context::_clients;

std::list<Channel>::iterator  Context::find_Channel(std::string name)
{
	std::list<Channel>::iterator it = Context::_channels.begin();
	for (; it != _channels.end(); ++it)
		if (it->getName() == name)
			break;
	return it;	
}

void Context::cmd_join(Client &client, std::string channel)
{
	std::list<Channel>::iterator it = find_Channel(channel);
	if (it == _channels.end())
	{
		_channels.push_back(Channel(channel, 'o'));
		it = (_channels.end())--;
	}
	(*it).addClient(client);
	client.setChannel(channel);
}

void Context::cmd_setNick(Client &client, std::string nick)
{
	client.setNick(nick);
}

void Context::cmd_setUserName(Client &client, std::string userName)
{
	client.setUserName(userName);
}

void Context::cop_kick(Client &client)
{
	std::list<Channel>::iterator it = find_Channel(client.getChannel());
	if (it != _channels.end())
	{
		(*it).removeClient(client);
		client.setChannel("");
		std::cout << client.getNick() << " has been kicked from " << client.getChannel() << std::endl;
	}
}

void Context::cop_invite(Client &client, std::string channel)
{
	//send msg to invite client, must respond y/N
	(void)client;
	(void)channel;
}

void Context::cop_topic(Client &client, std::string topic)
{
	(void)client;
	(void)topic;
}

void Context::cop_mode(Channel &channel, char c)
{
	if (c == 'i' || c == 't' || c == 'k' || c == 'o' || c == 'l')
		channel.setMode(c);
}
