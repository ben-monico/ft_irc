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

		//Getters
		std::string getNick() const;
		std::string getUserName() const;
		std::map<std::string, std::string> getChannels() const;
		std::string getChannelMode(std::string const & channel) const ;
		int getID() const;
		int	getInit() const;
		bool isOnChannel(std::string const &channel) const;

		std::vector<std::string> &getCmds();
		void addChannelMode(std::string const &channel, std::string const &mode);
		void removeChannel(std::string const &channel);
		void addChannelInvite(std::string const &channel);
		void removeChannelInvite(std::string const &channel);
		bool isInChannel(std::string const &channel);
		bool isInvitedToChannel(std::string const &channel);

	private:
		int _id;
		bool _init;
		std::string _nick;
		std::string _userName;
		std::map<std::string, std::string> _channelModes;
		std::vector<std::string> _channelInvites;
		std::vector<std::string> _cmds;
};

#endif