#include <Handler.hpp>
#include <Context.hpp>

void	Handler::extendFDsArray()
{

	struct pollfd	*tmpArray = new struct pollfd[_fdsSize * 2];

	if (!tmpArray)
		exit(pError("allocation error", "failed to allocate array of poll fds", 2));
	for (int i = 0; i < _fdsSize * 2; ++i)
	{
		i < _fdsSize ? tmpArray[i].fd = _pollFDsArray[i].fd : tmpArray[i].fd = -1;
		tmpArray[i].revents = _pollFDsArray[i].revents;
		tmpArray[i].events = POLLIN;
	}
	delete [] _pollFDsArray;
	_pollFDsArray = tmpArray;
	_fdsSize *= 2;
}

void	Handler::addToFDsArray(int newFD)
{

	if (!_pollFDsArray)
	{
		_pollFDsArray = new struct pollfd[_fdsSize];
		if (!_pollFDsArray)
			exit(pError("allocation error", "failed to allocate array of poll fds", 2));
		for (int i = 0; i < _fdsSize; ++i)
		{
			_pollFDsArray[i].fd = -1;
			_pollFDsArray[i].revents = 0;
		}
	}
	if (_fdsCount == _fdsSize)
		extendFDsArray();
	_pollFDsArray[_fdsCount].fd = newFD;
	_pollFDsArray[_fdsCount].events = POLLIN;
	_fdsCount += 1;
}

void	Handler::delFromFDsArray(int position)
{
	if (position > _fdsSize)
		return ;
	std::cout << "Connection closed with " << position << std::endl;
	close(_pollFDsArray[position].fd);	
	_pollFDsArray[position] = _pollFDsArray[--_fdsCount];
	Context::remove_client(position, _fdsCount);
}


void	Handler::handlePollResults()
{
	for (int i = 0; i < _fdsCount; ++i)
	{
		if (!_pollFDsArray[i].revents)
			continue ;
		if (!i)
			acceptIncomingConnection();
		else
			handleClientConnection(i);
	}
}