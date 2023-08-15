/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 16:43:26 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/15 17:24:52 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include <Bot.hpp>

int main(int ac, char **av)
{
	if ( ac != 2 && ac != 3 )
	{
		std::cerr << "bot : 1 or 2 argument required" << std::endl;
		return (1);
	}
	Bot bot;
	bot.setName(av[1]);
	if (ac == 3)
		bot.setPass(av[2]);
	bot.setCmd("bot.conf");
	try
	{
		bot.run();
	}
	catch (std::exception const &e)
	{
		std::cout << e.what() << std::endl;
	}


	return (0);
}
