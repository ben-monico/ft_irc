#include <Context.hpp>
#include <Handler.hpp>
#include <Channel.hpp>

void Context::chanop_kick(std::string const & channel, int client_id)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	//:kickerNick!~kickerUserName@hostname KICK #channel targetuser :You have been kicked by kickuser: Reason for kick.
	std::vector<Channel>::iterator it = find_chan_by_name(channel);
	if (it != _channels.end())
	{
		client->eraseChannel(channel);
	}
	server->sendAllBytes(client->getNick() + " KICK #" + channel + " " + client->getNick() + \
	 " :You have been kicked by " + client->getNick() + "\r\n", client->getId());
}

void Context::chanop_invite(int client_id, std::string channel)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	//if channel is invite only mode, invite must be chanop
	//ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
	//    ERR_NOTONCHANNEL                ERR_USERONCHANNEL
	//    ERR_CHANOPRIVSNEEDED
	//    RPL_INVITING                    RPL_AWAY
	(void)client;
	(void)channel;
}

void Context::chanop_topic(int client_id, std::string topic)
{
	std::vector<Client>::iterator client = find_client_by_id(client_id);
	(void)client;
	(void)topic;
	// ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
	//    RPL_NOTOPIC                     RPL_TOPIC
	//    ERR_CHANOPRIVSNEEDED
}

void Context::chanop_mode(Channel &channel, char c, std::string const & msg) 
{
	//check msg in case of additional params
	if (c == 'i' || c == 't' || c == 'k' || c == 'o' || c == 'l')
		channel.setMode(c);
	(void) msg;
}

