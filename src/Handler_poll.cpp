/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler_poll.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 16:48:35 by bcarreir          #+#    #+#             */
/*   Updated: 2023/06/01 16:48:42 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Handler.hpp>

void	Handler::extendFDsArray(int &fdsSize)
{

	struct pollfd	*tmpArray = new struct pollfd[fdsSize * 2];

	for (int i = 0; i < fdsSize * 2; ++i)
	{
		i < fdsSize ? tmpArray[i].fd = _pollFDsArray[i].fd : tmpArray[i].fd = -1;
		i < fdsSize ? tmpArray[i].revents = _pollFDsArray[i].revents : 0;
		tmpArray[i].events = POLLIN;
	}
	delete [] _pollFDsArray;
	_pollFDsArray = tmpArray;
	fdsSize *= 2;
}

void	Handler::addToFDsArray(int &fdsCount, int &fdsSize, int newFD)
{

	if (!_pollFDsArray)
	{
		_pollFDsArray = new struct pollfd[fdsSize];
		for (int i = 0; i < fdsSize; ++i)
			_pollFDsArray[i].fd = -1;
	}
	if (fdsCount == fdsSize)
		extendFDsArray(fdsSize);
	_pollFDsArray[fdsCount].fd = newFD;
	_pollFDsArray[fdsCount].events = POLLIN;
	fdsCount += 1;
}

void	Handler::delFromFDsArray(int &fdsCount, int &fdsSize, int position)
{
	if (position > fdsSize)
		return ;
	_pollFDsArray[position] = _pollFDsArray[--fdsCount];
	//set client id to new position no spaguetti forgetti
}


void	Handler::handlePollResults(int &fdsSize, int &fdsCount)
{
	for (int i = 0; i < fdsCount; ++i)
	{
		if (!_pollFDsArray[i].revents)
			continue ;
		if (!i) //is self aka listener fd
			acceptIncomingConnection(fdsSize, fdsCount);
		else
			handleClientConnection(fdsSize, fdsCount, i);
	}
}