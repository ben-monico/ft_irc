#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <vector>
# include <ircserv.hpp>
# include <map>

class Handler;
class Client;
class Channel
{
	public:
		Channel(std::string name);
		~Channel();

		//Setters
		void setTopic(std::string const& topic);
		void setUserLimit(int limit);
		void setKey(std::string const& key);
		void toggleRestrictTopic();
		void toggleInviteOnly();

		//Getters
		void getModes(int client_id, Handler *server, std::string const& host) const;
		std::string getName() const;
		std::string getTopic() const;
		int getUserLimit() const;
		int getUserCount() const;
		bool getInviteOnly() const;
		bool getTopicOpOnly() const;
		std::map<int, std::string> getUsersIn();
		std::string getKey() const;
		bool isFull() const;
		bool isUserInChannel(int userID);

		//Channel functions
		void broadcastMsg(std::string const &msg, Handler *server, int senderID);
		void addClient(int id, const std::string &mode);
		void removeClient(int id);
		void changeClientMode(int id, const std::string &mode);
		void autoOp();
		void replaceClientID(int old_id, int new_id);


	private:
		std::map<int, std::string> _usersID;
		std::string _name;
		std::string _topic;
		int _userLimit;
		int _userCount;
		int _chanOpCount;
		bool _inviteOnly;
		bool _topicOpOnly;
		std::string _key;
};

#endif