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
		int _id; //is his index in pollfds
		bool _init;
		std::string _nick;
		std::string _userName;
		std::string _channel;
		std::string _role;
};

#endif