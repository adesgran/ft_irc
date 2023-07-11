/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/01 13:57:01 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/02 13:44:40 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <irc.hpp>
#include <User.hpp>

int main( void )
{
	User user(1);
	std::string line;

		std::cout << "CLIENT >> ";
	while (std::getline(std::cin, line))
	{
		//read_msg(line);
		std::cout << "CLIENT >> ";
	}

	return (0);
};
