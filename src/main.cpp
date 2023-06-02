/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leferrei <leferrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 23:58:54 by bcarreir          #+#    #+#             */
/*   Updated: 2023/06/02 18:51:39 by leferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Handler.hpp>
#include <ircserv.hpp>

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Incorrect arguments" << std::endl;
		return (1);
	}
	Handler	server(argv[1]);
	server.init();
	server.start();
}
