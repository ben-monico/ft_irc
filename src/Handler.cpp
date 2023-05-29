/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 14:36:26 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/29 14:39:25 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLER_CPP
# define HANDLER_CPP

# include <ft_irc.h>
# include <Context.hpp>

//Static backend handler
class Handler 
{
	private:

	public:
		static void send_all(std::string &to_send);
		static void recv_all(std::string &to_recv);	
};

#endif