/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/21 14:09:10 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Message.hpp>

Message::Message(void)
{
}

Message::Message(const Message &message)
{
	*this = message;
}

Message::~Message(void)
{
};

Message &Message::operator=(const Message &message)
{
	if ( this == &message )
		return ( *this );
	return (*this);
}

void	Message::read( std::string message )
{
	std::cout << message << std::endl;
}

// Parsing ----------------------------------------------

void	Message::_cmdparser(std::string cmdline, User &user)
{
    std::vector<std::string> cmd_lines = _split(cmdline, '\n');
	for (std::vector<std::string>::iterator	line = cmd_lines.begin();
			line != cmd_lines.end();
			line++)
	{
		std::vector<std::string>	cmd_arg = _split(*line, ' ');
		switch (_cmdMap[cmd_arg[0]])
		{
			case CAP:
				_cmdName = _cmdMap[cmd_arg[0]];
				_cap(cmd_arg, user);
				break;
			case NICK:
				_cmdName = _cmdMap[cmd_arg[0]];
				_nick(cmd_arg, user);
				break;
			case USER:
				_cmdName = _cmdMap[cmd_arg[0]];
				_user(cmd_arg, user);
				break;
			default:
				_cmdName = ERR_UNKNOWNCMD;
				break;
		}
	}
}

std::vector<std::string>	Message::_split(const std::string& str, char sep)
{
    std::vector<std::string> tokens;
 
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, sep)) {
        tokens.push_back(token);
    }
 
    return tokens;
}

// IRC commands -----------------------------------------

void	Message::_cmdInit()
{
	_cmdMap["CAP"] = CAP;
	_cmdMap["NICK"] = NICK;
	_cmdMap["USER"] = USER;
}

void	Message::_cap(std::vector<std::string> arg, User &user)
{
	std::cout << "	*CAP cmd detected*\n";
	(void)arg;
	(void)user;
}

void	Message::_nick(std::vector<std::string> arg, User &user)
{
	std::cout << "	*NICK cmd detected*\n";
	user.setNickname(arg[1]);
}

void	Message::_user(std::vector<std::string> arg, User &user)
{
	std::cout << "	*USER cmd detected*\n";
	(void)arg;
	(void)user;
}
