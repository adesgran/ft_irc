/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/01 15:22:19 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/01 15:55:59 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <User.hpp>
#include <sstream>
#include <vector>

std::vector<std::string>	tokenize(std::string line)
{
	std::vector<std::string> res;
	std::stringstream ss(line);
	std::string	str;
	while( std::getline(ss, str, ' ') )
	{
		if (str.size() > 0)
			res.push_back(str);
	}
	return (res);
}

void	read_cmd( std::string cmd )
{
	std::vector<std::string> tokens = tokenize(cmd);
	if (tokens.size() < 1)
		return;
	for ( std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); it++)
	{
		std::cout << "<" << *it << "> ";
	}
	std::cout << std::endl;
	if (tokens[0] == "USER")
	{
		std::cout << "USER cmd detected" << std::endl;
	}
	else
	{
		std::cout << tokens[0] << "is an UNKNOWN command" << std::endl;
	}
}
