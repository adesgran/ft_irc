/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 16:43:26 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/01 19:05:15 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include <Bot.hpp>

int main(int ac, char **av)
{
	if ( ac != 2 )
	{
		std::cerr << "bot : 1 argument required" << std::endl;
		return (1);
	}
	Bot bot;
	bot.conf(av[1]);
	bot.run();


	return (0);
}
