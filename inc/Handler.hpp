/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcarreir <bcarreir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 14:36:26 by bcarreir          #+#    #+#             */
/*   Updated: 2023/05/31 16:02:55 by bcarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLER_HPP
# define HANDLER_HPP

#include <ircserv.hpp>

# include <Context.hpp>

//Static backend handler
class Handler 
{
	private:

	public:
		static void send_all(int client_id, std::string const &to_send);
		static void recv_all(int client_id);	
};

#endif