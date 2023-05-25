#ifndef  CONTEXT_HPP
# define CONTEXT_HPP

# include <iostream>
# include <string>
# include <vector>
# include "Channel.hpp"
# include "Client.hpp"


class Context
{
	public:
	//Commands
		static void cmd_join(Client &client, std::string channel);
		static void cmd_setNick(Client &client, std::string nick);
		static void cmd_setUserName(Client &client, std::string userName);

	//Channel operations
		static void cop_kick(Client &client);
		static void cop_invite(Client &client, std::string channel);
		static void cop_topic(Client &client, std::string topic);
		static void cop_mode(Channel &channel, char c);

		static std::vector<Channel>::iterator  find_Channel(std::string name);

	private:
		static std::vector<Channel>	_channels;
		static std::vector<Client>	_clients;
};

#endif