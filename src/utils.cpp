/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 17:20:46 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/31 15:37:54 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ircserv.hpp>


void ft_bzero(void *s, size_t n)
{
	char	*p = (char *)s;
	for (size_t i = 0; i < n; i++)
		p[i] = 0;
}