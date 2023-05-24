#include "Channel.hpp"

// Constructors
Channel::Channel()
{
	std::cout << "\e[0;33mDefault Constructor called of Channel\e[0m" << std::endl;
}

Channel::Channel(const Channel &copy)
{
	(void) copy;
	std::cout << "\e[0;33mCopy Constructor called of Channel\e[0m" << std::endl;
}


// Destructor
Channel::~Channel()
{
	std::cout << "\e[0;31mDestructor called of Channel\e[0m" << std::endl;
}


// Operators
Channel & Channel::operator=(const Channel &assign)
{
	(void) assign;
	return *this;
}

