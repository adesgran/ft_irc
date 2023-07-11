/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message_reader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 11:50:23 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/02 14:04:48 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <User.hpp>
#include <sstream>
#include <vector>

std::vector<std::string>	tokenize(std::string &line)
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

void	read_msg( std::string &msg, User &user, Server &server )
{
	(void)user;
	(void)server;

	std::vector<std::string> args = tokenize(msg);
	if (args.size() < 1)
		return;
	for ( std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++)
	{
		std::cout << "<" << *it << "> ";
	}
	std::cout << std::endl;
	if (args[0] == "USER")
	{
		std::cout << "USER instruction detected" << std::endl;
	}
	else
	{
		std::cout << args[0] << "is an UNKNOWN command" << std::endl;
	}
}

