#ifndef  CONTEXT_HPP
# define CONTEXT_HPP

# include <iostream>
# include <string>
# include <vector>
#include <Client.hpp>

class Channel;
class Handler;

//Static management
class Context
{
	public:
		static std::string	welcome;
		static void add_client(Client client);
		static void	remove_client(int position, int fdsCount);
		static void setHostname(std::string hostname);
		static void	verifyLoginInfo(int id);
	//Commands
		static void cmd_join(Client &client, std::string const &channel);
		static void cmd_setNick(Client &client, std::string nick);
		static void cmd_setUserName(Client &client, std::string userName);
		static void cmd_sendPM(Client &client, std::string const &msg);
		static void	execClientCmds(int id);
	//Channel operations
		static void chanop_kick(std::string const & channel, Client &client);
		static void chanop_invite(Client &client, std::string channel);
		static void chanop_topic(Client &client, std::string topic);
		static void chanop_mode(Channel &channel, char c, std::string const & msg);
	//Getters and checkers
		static std::vector<Channel>::iterator	find_chan_by_name(std::string name);
		static std::vector<Client>::iterator	find_client_by_id(int id);
		static std::vector<Client>::iterator 	find_client_by_nick(const std::string &nick);
		Client									&get_client_by_id(int id);

		static bool isUserInVector(std::vector<Client>::iterator userGot) { return (userGot != _clients.end()); }
		static void	setServerPtr(Handler *serverPtr);
	//Command Responses
		static void CHAN_RPL_TEMPLATE(std::string code, Channel &channel, std::string msg, int client_id);
		static void USR_RPL_TEMPLATE(std::string code, std::string msg, std::string nick, int client_id);
		static void RPL_TOPIC(int client_id, Channel &channel);
		static void RPL_NAMREPLY(int client_id, Channel &channel);
		static void RPL_ENDOFNAMES(int client_id, Channel &channel);
		static void ERR_PASSWDMISMATCH(int client_id);


	private:
		static std::string 			_hostname;
		static Handler				*server;
		static std::vector<Client>	_clients;
		static std::vector<Channel>	_channels;
};

#endif