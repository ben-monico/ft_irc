/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/24 17:52:03 by bcarreir          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2023/05/31 18:41:05 by bcarreir         ###   ########.fr       */
=======
/*   Updated: 2023/06/01 16:57:27 by bcarreir         ###   ########.fr       */
>>>>>>> bronadofix
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <ircserv.hpp>


class Channel;

class Client
{
	public:
		Client(int id) \
			:_id(id), _init(false), _channel(""), _role("default") { }
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
		int	getInit() { return (_init); }
		void init(std::string nick, std::string userName) { _nick = nick; _userName = userName; _init = true; }

	private:
		int _fd;
		bool _init;
		std::string _nick;
		std::string _userName;
		int _id; //is his index in pollfds
		std::string _channel;
		std::string _role;
};

#endif