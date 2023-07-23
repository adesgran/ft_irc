/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/23 13:39:54 by mchassig         ###   ########.fr       */
/*   Updated: 2023/07/23 02:24:12 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Message.hpp>

Message::Message(void)
{
	_cmdMap["CAP"] = CAP;
	_cmdMap["NICK"] = NICK;
	_cmdMap["USER"] = USER;
	this->_output = std::string("");
}

Message::Message(User *sender): _sender(sender)
{
	_cmdMap["CAP"] = CAP;
	_cmdMap["NICK"] = NICK;
	_cmdMap["USER"] = USER;
	this->_output = std::string("");
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
	this->_sender = message._sender;
	this->_input = message._input;
	this->_output = message._input;
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
	return (this->_output);
}

void		Message::clearOutputMsg()
{
	this->_output.clear();
}


// IRC commands -----------------------------------------

void	Message::nick(std::vector<std::string> arg)
{
	std::cout << "	*Message class: NICK cmd detected*\n";

	// if (arg.size() < 2)
		// ERR_NONICKNAMEGIVEN	=> should ignore the command

	// if (arg[1].find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[]{}\\|"))
		// ERR_ERRONEUSNICKNAME	=> should ignore the command

	// si nickname deja utilise sur le server
		// ERR_NICKNAMEINUSE	=> should ignore the command


	_sender->setNickname(arg[1]);
	std::cout << "		nickname: " << _sender->getNickname() << std::endl;

}

void	Message::user(std::vector<std::string> arg)
{
	std::cout << "	*Message class: USER cmd detected*\n";
	
	//if (arg.size() < 5)
		// ERR_NEEDMOREPARAMS => server should reject command
	//if (_sender->isWelcomed())
		// ERR_ALREADYREGISTERED => attempt should fail
	
	std::vector<std::string>::iterator	it = ++arg.begin();
	/* username */
	_sender->setUsername(*it++);
	/* mode */
	std::stringstream	ss;
	int	mode;
	ss << *it++;
	ss >> mode;
	_sender->setMode(mode);
	/* unused */
	it++;
	/* real name */
	std::string	realName = *it++;
	if (realName[0] == ':')
		realName.erase(0, 1);
	while (it != arg.end())
	{
		realName += " ";
		realName += *it++;
	}
	_sender->setRealname(realName);
	

	std::cout << "		user: " << _sender->getUsername()
			<< "\n		mode: " << _sender->getMode()
			<< "\n		real name: " << _sender->getRealname()
					<< std::endl;

	_sender->welcome();
	_output += "001 Welcome to the <networkname> Network, " + _sender->getNickname() + "\n"; //<nick>[!<user>@<host>]
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
