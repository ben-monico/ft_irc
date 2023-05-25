/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 22:49:40 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/25 22:52:55 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <vector>
# include <ft_irc.h>

class Client;

class Channel
{
	public:
		Channel(std::string name, char mode) : _name(name), _mode(mode), _topic("general") { }
		~Channel() {}

		void addClient(Client &client) { _clients.push_back(client); }
		void removeClient(Client &client)
		{
			std::vector<Client>::iterator it = _clients.begin();
			for (; it != _clients.end(); ++it)
				if (it->getNick() == client.getNick())
					break;
			if (it != _clients.end())
				_clients.erase(it);
		}

		void setMode(char mode) { _mode = mode; }
		void setTopic(std::string topic) { _topic = topic; }

		std::string getName() const { return _name; }
			
	private:
		std::vector<Client> _clients;
		std::string _name;
		char _mode;
		std::string _topic;
		std::vector<Client> _operators;
		
};

#endif