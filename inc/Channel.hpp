/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 22:49:40 by bcarreir          #+#    #+#             */
/*   Updated: 2023/06/05 17:47:35 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <vector>
# include <ircserv.hpp>
# include <Client.hpp>


class Client;

class Channel
{
	public:
		Channel(std::string name, char mode) : _name(name), _mode(mode), _topic("NOTOPIC") { }
		~Channel() {}

		void setMode(char mode) { _mode = mode; }
		void setTopic(std::string topic) { _topic = topic; }

		std::string getName() const { return _name; }
		std::string getTopic() const { return _topic; }
			
	private:
		std::string _name;
		char _mode;
		std::string _topic;
		std::vector<Client> _operators;
		
};

#endif