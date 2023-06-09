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

		void setTopic(std::string const& topic);
		void setUserLimit(int limit);
		void setKey(std::string const& key);
		void toggleRestrictTopic();
		void toggleInviteOnly();
		void decrementUserCount(int id);
		void incrementUserCount(int id);

		void getModes(int client_id, Handler *server, std::string const& host) const;
		std::string getName() const;
		std::string getTopic() const;
		int getUserLimit() const;
		int getUserCount() const;
		bool getInviteOnly() const;
		bool getTopicOpOnly() const;
		bool isFull() const;
		std::string getKey() const;
		void broadcastMsg(std::string const &msg, Handler *server, int senderID);
		void replaceClientID(int old_id, int new_id);
		std::vector<int> _usersID;


	private:
		std::string _name;
		std::string _topic;
		int _userLimit;
		int _userCount;
		bool _inviteOnly;
		bool _topicOpOnly;
		std::string _key;
};

#endif