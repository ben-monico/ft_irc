/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 17:20:46 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/31 19:05:11 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ircserv.hpp>

void ft_bzero(void *s, size_t n)
{
	char	*p = (char *)s;
	for (size_t i = 0; i < n; i++)
		p[i] = 0;
}

size_t ft_strlen(char *str)
{
	int i = -1;

	if (!str || !*str)
		return (0);
	while (str[++i]) {}
	return (i);
}

size_t lenToIRCEOL(char *str)
{
	int i = -1;

	if (!str || !*str)
		return (0);
	while (str[++i])
	{
		if (str[i] == '\r' && str[i + 1] == '\n')
			return (i);
	}
	return (i);
}	