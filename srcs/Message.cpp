/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/21 18:38:43 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Message.hpp>

Message::Message(void)
{
	_cmdMap["CAP"] = CAP;
	_cmdMap["NICK"] = NICK;
	_cmdMap["USER"] = USER;
}

Message::Message(std::string input_buffer, User *sender): _sender(sender), _input(input_buffer)
{
	_cmdMap["CAP"] = CAP;
	_cmdMap["NICK"] = NICK;
	_cmdMap["USER"] = USER;
	_parseInput(_split(_input, '\n'));
}

Message::Message(const Message &message)
{
	*this = message;
}

Message::~Message(void)
{
	(void)_sender;
};

Message &Message::operator=(const Message &message)
{
	if ( this == &message )
		return ( *this );
	return (*this);
}

// void	Message::read( std::string message )
// {
// 	std::cout << message << std::endl;
// }

void		Message::setInputMsg(std::string &input_buffer)
{
	_input = input_buffer;
	_parseInput(_split(_input, '\n'));
}

std::string	Message::getInputMsg() const
{
	return (_input);
}

std::string	Message::getOutputMsg() const
{
	return (_output);
}

// IRC commands -----------------------------------------

void	Message::nick(std::vector<std::string> arg)
{
	std::cout << "	*Message class: NICK cmd detected*\n";
	_sender->setNickname(arg[1]);
}

void	Message::user(std::vector<std::string> arg)
{
	std::cout << "	*Message class: USER cmd detected*\n";
		if (!_sender->isWelcomed())
	{
		_sender->welcome();
		_output += "001 Welcome to the <networkname> Network, " + _sender->getNickname() + "\n"; //<nick>[!<user>@<host>]
	}
	(void)arg;
}

void		Message::privmsg(std::vector<std::string> arg)
{
	std::cout << "	*Message class: PRIVMSG cmd detected*\n";
	(void)arg;
}

void		Message::kick(std::vector<std::string> arg)
{
	std::cout << "	*Message class: KICK cmd detected*\n";
	(void)arg;
}

void		Message::invite(std::vector<std::string> arg)
{
	std::cout << "	*Message class: INVITE cmd detected*\n";
	(void)arg;
}

void		Message::topic(std::vector<std::string> arg)
{
	std::cout << "	*Message class: TOPIC cmd detected*\n";
	(void)arg;
}

void		Message::mode(std::vector<std::string> arg)
{
	std::cout << "	*Message class: MODE cmd detected*\n";
	(void)arg;
}

void		Message::ping(std::vector<std::string> arg)
{
	std::cout << "	*Message class: PING cmd detected*\n";
	(void)arg;
}


// Utils ----------------------------------------------
void	Message::_parseInput(std::vector<std::string> input_lines)
{
	for (std::vector<std::string>::iterator	line = input_lines.begin();
			line != input_lines.end();
			line++)
	{
		std::vector<std::string>	cmd_arg = _split(*line, ' ');
		switch (_cmdMap[cmd_arg[0]])
		{
			case NICK:
				nick(cmd_arg);
				break;
			case USER:
				user(cmd_arg);
				break;
			case PRIVMSG:
				privmsg(cmd_arg);
				break;
			case KICK:
				kick(cmd_arg);
				break;
			case TOPIC:
				topic(cmd_arg);
				break;
			case MODE:
				mode(cmd_arg);
				break;
			case PING:
				ping(cmd_arg);
				break;
			case CAP:
				break;
			default:
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
