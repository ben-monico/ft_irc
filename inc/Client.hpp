/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/24 17:52:03 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/31 15:39:26 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <ircserv.hpp>


class Channel;

class Client
{
	public:
		Client(std::string nick, std::string username, int id) \
			: _nick(nick), _userName(username), _id(id), _channel(""), _role("default") { }
		~Client() { }
		
		//Setters
		void setNick(std::string nick) { _nick = nick; }
		void setUserName(std::string userName) { _userName = userName; }
		void setChannel(std::string channel) { _channel = channel; }
		//Getters
		std::string getNick() const { return _nick; }
		std::string getChannel() const { return _channel; }
		std::string getRole() const { return _role; }
		int getId() const { return _id; }
		

	private:
		std::string _nick;
		std::string _userName;
		int _id;
		std::string _channel;
		std::string _role;
};

#endif