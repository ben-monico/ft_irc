#include <Bot.hpp>
#include <Handler.hpp>
#include <Context.hpp>
#include <Client.hpp>
#include <Channel.hpp>

Handler	*Bot::server = NULL;

void	Bot::setServerPtr(Handler *serv)
{
	server = serv;
}

void	Bot::help(int id)
{
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :SERVER:\r\n", id);
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :Available Commands:\r\n", id);
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :\tJOIN MODE NICK PART PRIVMSG USER\r\n", id);
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :Available Commands for Channel Operators:\r\n", id);
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :\tINVITE KICK MODE\r\n", id);
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :Available Modes:\r\n", id);
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :\ti k l o t\r\n", id);
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :BOT:\r\n", id);
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :Available commands:\r\n", id);
	server->sendAllBytes(":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :\t//help //users //channels\r\n", id);
}

void Bot::channels(int id)
{
	std::vector<Channel>::iterator it = Context::getChannels().begin();
	std::string chanList = ":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :Available Channels: \t";
	for (; it != Context::getChannels().end(); ++it)
		chanList += " #" + it->getName();
	server->sendAllBytes(chanList + "\r\n", id);
}

void Bot::users(int id)
{
	std::vector<Client>::iterator it = Context::getClients().begin();
	std::string userList = ":BOT!BOT@localhost PRIVMSG " + Context::find_client_by_id(id)->getNick() + " :Available Users:\t";
	for (; it != Context::getClients().end(); ++it)
		userList += " " + it->getNick();
	server->sendAllBytes(userList + "\r\n", id);
}