#ifndef BOT_HPP
# define BOT_HPP

#include <iostream>

class Handler;

class Bot 
{
	private:
		static Handler	*server;

	public:
		static void help(int id);
		static void setServerPtr(Handler *serv);
		static void users(int id);
		static void channels(int id);
};

#endif