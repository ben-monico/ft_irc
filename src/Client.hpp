/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/24 17:52:03 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/25 16:50:40 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include "Channel.hpp"

class Channel;

class Client
{
	public:
		Client(std::string nick) : _nick(nick), _channel("") { }
		~Client() { }
		std::string getNick() const { return _nick; }
		void setNick(std::string nick) { _nick = nick; }
		void setUserName(std::string userName) { _userName = userName; }
		void setChannel(std::string channel) { _channel = channel; }
		std::string getChannel() const { return _channel; }
		std::string getRole() const { return _role; }

	private:
		std::string _userName;
		std::string _nick;
		std::string _channel;
		std::string _role;
};

#endif