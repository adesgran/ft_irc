/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:18:05 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/13 02:36:04 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <Server.hpp>

int main(int ac, char **av)
{
	if (ac != 3 && ac != 2)
	{
		std::cerr << "irc: at least 1 arguments expected" << std::endl << "irc <port> [<password>]" << std::endl;
		return (1);
	}

	std::stringstream ss1(av[1]);

	int port;
	try
	{
		for ( size_t i = 0; av[1][i]; i++)
		{
			if ( av[1][i] < '0' || av[1][i] > '9' )
			{
				std::cerr << "irc: " << av[1] << " is an invalid argument <port>: not a positive integer" << std::endl;
				return (1);
			}
		}
		ss1 >> port;
		if ( port < 1024 || port > 65535 )
		{
				std::cerr << "irc: " << av[1] << " is an invalid argument <port>: port must be between 1024 and 65535" << std::endl;
				return (1);
		}
	}
	catch ( std::exception &e )
	{
		std::cout << "Exception Catched : " << e.what() << std::endl;
	}

	std::string pass;
	if (ac == 3)
		pass = av[2];

	Server	server(port);
	server.setPassword(pass);

	
	try
	{
		server.run();
	}
	catch ( std::exception &e )
	{
		std::cout << "Exception Catched : " << e.what() << std::endl;
	}

	return (0);
}
