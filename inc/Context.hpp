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
		static void	verifyLoginInfo(int );
		static void add_client(int);
		static void	remove_client(int, int);
		static void addClientToChannel(int, std::string const&, std::string const&);
		static void removeClientFromChannel(int, std::string const&);

	//Commands
		static void cmd_join(int, std::string const&);
		static void cmd_setNick(int, std::string );
		static void cmd_setUserName(int, std::string );
		static void cmd_sendPM(int, std::string const&);
		static void	execClientCmds(int d);

	//Channel operations
		static void chanop_kick(std::string const& , int);
		static void chanop_invite(int, std::string );
		static void chanop_topic(int, std::string const& );
		static void chanop_mode(int, std::string const& , std::string const& );

	//Getters and checkers
		static std::vector<Channel>::iterator	find_chan_by_name(std::string );
		static std::vector<Client>::iterator	find_client_by_id(int);
		static std::vector<Client>::iterator 	find_client_by_nick(std::string const&);

		static bool isUserInVector(std::vector<Client>::iterator );
		static void	setServerPtr(Handler *);

	//Command Responses
		static void CHAN_RPL_TEMPLATE(std::string , Channel&, std::string , int);
		static void USR_RPL_TEMPLATE(std::string , std::string , std::string , int);
		static void RPL_TOPIC(int, Channel&);
		static void RPL_NOTOPIC(int, Channel&)
;		static void RPL_NAMREPLY(int, Channel&);
		static void RPL_ENDOFNAMES(int, Channel&);
		static void ERR_PASSWDMISMATCH(int);
		static void RPL_WELCOME(int);


	private:
		static std::string 			_hostname;
		static Handler				*server;
		static std::vector<Client>	_clients;
		static std::vector<Channel>	_channels;
};

#endif