/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 18:43:31 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/25 19:03:14 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"
#include "Context.hpp"

void parse_recv(std::string str)
{
	Client client("Beano");
	if (str.rfind("/join #", 0) != std::string::npos)
	{
		if (str[7])
		{	
			std::string channel = str.substr(7);
			Context::cmd_join(client, channel);
		}
		
	}
	std::cout << str << std::endl;
}