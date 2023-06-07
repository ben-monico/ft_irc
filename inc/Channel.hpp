#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <vector>
# include <ircserv.hpp>
# include <map>


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
		void decrementUserCount();
		void incrementUserCount();

		std::string getName() const;
		std::string getTopic() const;
		int getUserLimit() const;
		int getUserCount() const;
		bool getInviteOnly() const;
		int getUserCount() const;
		bool getTopicOpOnly() const;
		std::string getKey() const;
			
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