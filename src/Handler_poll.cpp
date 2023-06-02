/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler_poll.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leferrei <leferrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 16:48:35 by bcarreir          #+#    #+#             */
/*   Updated: 2023/06/02 18:08:00 by leferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Handler.hpp>
#include <Context.hpp>

void	Handler::extendFDsArray()
{

	struct pollfd	*tmpArray = new struct pollfd[_fdsSize * 2];

	for (int i = 0; i < _fdsSize * 2; ++i)
	{
		i < _fdsSize ? tmpArray[i].fd = _pollFDsArray[i].fd : tmpArray[i].fd = -1;
		i < _fdsSize ? tmpArray[i].revents = _pollFDsArray[i].revents : 0;
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
		for (int i = 0; i < _fdsSize; ++i)
			_pollFDsArray[i].fd = -1;
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
		if (!i) //is self aka listener fd
			acceptIncomingConnection();
		else
			handleClientConnection(i);
	}
}