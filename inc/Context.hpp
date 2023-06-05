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
		static void	verifyLoginInfo(int id);
		static void add_client(int client_id);
		static void	remove_client(int id_erase, int id_replace);
		static void addClientToChannel(int client_id, std::string const&, std::string const&);
		static void removeClientFromChannel(int client_id, std::string const&);

	//Commands
		static void cmd_join(int client_id, std::string const &channel);
		static void cmd_setNick(int client_id, std::string nick);
		static void cmd_setUserName(int client_id, std::string userName);
		static void cmd_sendPM(int client_id, std::string const &msg);
		static void	execClientCmds(int id);

	//Channel operations
		static void chanop_kick(std::string const & channel, int client_id);
		static void chanop_invite(int client_id, std::string channel);
		static void chanop_topic(int client_id, std::string topic);
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
		static void RPL_WELCOME(int client_id);


	private:
		static std::string 			_hostname;
		static Handler				*server;
		static std::vector<Client>	_clients;
		static std::vector<Channel>	_channels;
};

#endif