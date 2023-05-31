/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/24 19:09:34 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/31 18:49:22 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Context.hpp>

//ERRORS ON PAGE 43

std::vector<Channel> Context::_channels;
std::vector<Client> Context::_clients;
std::string Context::welcome = "Welcome to ft_irc\r\n";

std::vector<Channel>::iterator  Context::find_chan_by_name(std::string name)
{
	std::vector<Channel>::iterator it = Context::_channels.begin();
	for (; it != _channels.end(); ++it)
		if (it->getName() == name)
			break;
	return it;	
}

std::vector<Client>::iterator  Context::find_client_by_id(int id)
{
	std::vector<Client>::iterator it = Context::_clients.begin();
	for (; it != _clients.end(); ++it)
		if (it->getId() == id)
			break;
	return it;	
}

//Commands
void Context::cmd_join(Client &client, std::string const &channel)
{
	//check if invite only, or for pwd
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it == _channels.end())
	{
		_channels.push_back(Channel(channel, 'o'));
		it = (_channels.end())--;
		
	}
	(*it).addClient(client);
	client.setChannel(channel);
	RPL_TOPIC(client.getId(), *it);
	
	//display chan topic using RPL_TOPIC
	// /ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
	// ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
	// ERR_CHANNELISFULL               ERR_BADCHANMASK
	// ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
	// RPL_TOPIC
}

void Context::cmd_setNick(Client &client, std::string nick)
{
	//NICK | ERR_NICKCOLLISION ERR_NONICKNAMEGIVEN ERR_NICKNAMEINUSE ERR_ERRONEUSNICKNAME
	client.setNick(nick);
}

void Context::cmd_setUserName(Client &client, std::string userName)
{
	client.setUserName(userName);
}

void Context::cmd_sendPM(Client &client, std::string const & msg)
{
	//get target in message, send() desired PM
	(void)client;
	(void)msg;
	// ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
	//    ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
	//    ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
	//    ERR_NOSUCHNICK
	//    RPL_AWAY
}

//Channel operations
void Context::chanop_kick(Client &client)
{
	//only chanop can kick
	std::vector<Channel>::iterator it = find_chan_by_name(client.getChannel());
	if (it != _channels.end())
	{
		(*it).removeClientFromChannel(client.getNick());
		client.setChannel("");
	}
	//ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
	//    ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
	//    ERR_NOTONCHANNEL
}

void Context::chanop_invite(Client &client, std::string channel)
{
	//if channel is invite only mode, invite must be chanop
	//ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
	//    ERR_NOTONCHANNEL                ERR_USERONCHANNEL
	//    ERR_CHANOPRIVSNEEDED
	//    RPL_INVITING                    RPL_AWAY
	(void)client;
	(void)channel;
}

void Context::chanop_topic(Client &client, std::string topic)
{
	(void)client;
	(void)topic;
	// ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
	//    RPL_NOTOPIC                     RPL_TOPIC
	//    ERR_CHANOPRIVSNEEDED
}


void Context::chanop_mode(Channel &channel, char c, std::string const & msg) 
{
	//check msg in case of additional params
	if (c == 'i' || c == 't' || c == 'k' || c == 'o' || c == 'l')
		channel.setMode(c);
	(void) msg;
}

void Context::add_client(Client client)
{
	_clients.push_back(client);
}

//Command responses

void Context::RPL_TOPIC(int client_id, Channel &channel)
{
	(void)channel;
	(void)client_id;
	// Handler::send_all(client_id, channel.getName() + " : " + channel.getTopic() + "\r\n");
}