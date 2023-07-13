/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:18:05 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/13 15:36:36 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <stdexcept>
#include <Server.hpp>

int main(void)
{
	Server	server;
	
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
