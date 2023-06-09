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
		static void	setServerPtr(Handler *);
		static void	verifyLoginInfo(int );
		static void add_client(int);
		static void	remove_client(int, int);
		static void addClientToChannel(int, std::string const&, std::string const&);
		static void removeClientFromChannel(int, std::string const&);

	//Command parsers
		static void	execClientCmds(int client_id);
		static void	parseJoin(std::vector<Client>::iterator client, std::string &cmd);
		static void	parseInvite(std::vector<Client>::iterator client, std::string &cmd);
		static void	parseWho(std::vector<Client>::iterator client, std::string &cmd);
		static void	parseMode(std::vector<Client>::iterator client, std::string &cmd);
		static void	parseNick(std::vector<Client>::iterator client, std::string &cmd);
		static void	parseTopic(std::vector<Client>::iterator client, std::string &cmd);
		static void	parseList(std::vector<Client>::iterator client, std::string &cmd);
		static void	parseKick(std::vector<Client>::iterator client, std::string &cmd);
		static void	parsePrivmsg(std::vector<Client>::iterator client, std::string &cmd);
		static void	parseQuit(std::vector<Client>::iterator client, std::string &cmd);
		static bool	verifyModeOptions(std::vector<std::string> &vec);

		
	//Commands
		static void	execModeOptions(std::vector<std::string> vec, std::vector<Client>::iterator client, const std::string &chan);
		static void cmd_join(int client_id, std::string const &channelName, std::string const& key);
		static void cmd_setNick(int client_id, std::string );
		static void cmd_setUserName(int client_id, std::string );
		static void cmd_sendPM(int client_id, std::string recipient, std::string const& msg);

	//Channel operations
		static void chanop_kick_user(int client_id, std::string const& channel, std::string const& target);
		static void chanop_invite_user(int client_id, std::string const& channel, std::string const& target);
		static void chanop_topic(int client_id, std::string const& channel, std::string const& newtopic);

		static void chanop_toggle_inviteonly(int, std::string );
		static void chanop_toggle_inviteonly(int, std::string, bool);
		static void chanop_toggle_topic_restriction(int, std::string const&, bool);
		static void chanop_mode(int, std::string const& , std::string const& );
		static void chanop_userlimit(int client_id, std::string const& channel, std::string const& userLimit);
		static void chanop_key(int client_id, std::string const& channel, std::string const& key);
		static void chanop_toggle_oppriv(int client_id, std::string const& channel, std::string const& targetNick, std::string toggle);


	//Getters and checkers
		static std::vector<Channel>::iterator	find_chan_by_name(std::string );
		static std::vector<Client>::iterator	find_client_by_id(int);
		static std::vector<Client>::iterator 	find_client_by_nick(std::string const&);

		static bool isUserInVector(std::vector<Client>::iterator );
		static bool isChannelInVector(std::vector<Channel>::iterator );


	//Command Responses
		static void CHAN_RPL_TEMPLATE(std::string , Channel&, std::string , int);
		static void USR_RPL_TEMPLATE(std::string , std::string , std::string , int);
		static void RPL_TOPIC(int, Channel&);
		static void RPL_NOTOPIC(int, Channel&)
;		static void RPL_NAMREPLY(int, Channel&);
		static void RPL_ENDOFNAMES(int, Channel&);
		static void RPL_WELCOME(int);
		static void ERR_NOTONCHANNEL(int, std::string const&);
		static void ERR_USERONCHANNEL(int, std::string const&, std::string const&);
		static void ERR_NOSUCHCHANNEL(int, std::string const&);
		static void ERR_NOSUCHNICK(int, std::string const&);
		static void ERR_CHANNELISFULL(int client_id, std::string const& channel_name);
		static void ERR_INVITEONLYCHAN(int client_id, std::string const& channel_name);
		static void ERR_BADCHANNELKEY(int client_id, std::string const& channel_name);
		static void ERR_PASSWDMISMATCH(int);
		static void ERR_CHANOPRIVSNEEDED(int, std::string const&);
		static void	ERR_UNRECOGNIZEDCMD(const int &client_id, const std::string &cmd, const std::string reason);
		static void RPL_WHOREPLY(int client_id, Channel &channel);
		static void RPL_CHANNELMODEIS(int client_id, Channel &channel);
		static void ERR_CANNOTSENDTOCHAN(int client_id, std::string const& channel_name);
		


	private:
		static std::string 			_hostname;
		static Handler				*server;
		static std::vector<Client>	_clients;
		static std::vector<Channel>	_channels;
};

#endif