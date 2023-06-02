#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <ircserv.hpp>
#include <map>
#include <vector>
class Channel;

class Client
{
	public:
		Client(int id) :_id(id), _init(false) { }
		~Client() { }
		void init(std::string nick, std::string userName);
		
		//Setters
		void setNick(std::string const &nick);
		void setUserName(std::string const &userName);
		void setID(int	id);
		void setMode(std::string const &channel, std::string const &mode);

		//Getters
		std::string getNick() const;
		std::string getUserName() const;
		std::vector<std::string> getChannels() const;
		std::string getChannelMode(std::string const & channel) const ;
		int getId() const;
		int	getInit() const;
		std::vector<std::string> &getCmds();

		void eraseChannel(std::string const &channel);

	private:
		std::vector<std::string> _cmds;
		std::vector<std::string> _channelInvites;
		std::map<std::string, std::string> _channelModes; //@ or + or nothing
		int _id; //is his index in pollfds
		bool _init;
		std::string _nick;
		std::string _userName;
};

#endif