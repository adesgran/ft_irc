/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/27 15:45:20 by mchassig         ###   ########.fr       */
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
};

Message &Message::operator=(const Message &message)
{
	if ( this == &message )
		return ( *this );
	this->_sender = message._sender;
	this->_server = message._server;
	this->_input = message._input;
	this->_output = message._input;
	return (*this);
}

// void	Message::read( std::string message )
// {
// 	std::cout << message << std::endl;
// }

void		Message::setInputMsg(std::string &input_buffer, Server *server)
{
	_input = input_buffer;
	_server = server;
	_parseInput(_split(_input, "\r\n"));
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


// Utils ----------------------------------------------
void	Message::_parseInput(std::vector<std::string> input_lines)
{
	for (std::vector<std::string>::iterator	line = input_lines.begin();
			line != input_lines.end();
			line++)
	{
		std::vector<std::string>	cmd_arg = _split(*line, " ");
		switch (_cmdMap[cmd_arg[0]])
		{
			case NICK:
				_nick(cmd_arg);
				break;
			case USER:
				_user(cmd_arg);
				break;
			case PRIVMSG:
				_privmsg(cmd_arg);
				break;
			case KICK:
				_kick(cmd_arg);
				break;
			case TOPIC:
				_topic(cmd_arg);
				break;
			case MODE:
				_mode(cmd_arg);
				break;
			case PING:
				_ping(cmd_arg);
				break;
			case CAP:
				break;
			default:
				break;
		}
	}
}
std::vector<std::string>	Message::_split(std::string str, std::string sep)
{
	std::vector<std::string> res;
    size_t pos = 0;
    while(pos < str.size()){
        pos = str.find(sep);
        res.push_back(str.substr(0,pos));
        str.erase(0, pos + sep.size()); // 3 is the length of the delimiter, "%20"
    }
    return (res);
}

// IRC commands -----------------------------------------
void	Message::_nick(std::vector<std::string> arg)
{
	std::cout << "	*Message class: NICK cmd detected*\n";

	if (arg.size() < 2)
	{
		_output = ITOA(ERR_NONICKNAMEGIVEN) + '\n'; //	=> should ignore the command
		return ;
	}
	if (arg[1].find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[]{}\\|") != std::string::npos)
	{
		_output = ITOA(ERR_ERRONEUSNICKNAME) + '\n';	// => should ignore the command
		return ;
	}
	// si nickname deja utilisé sur le server
	// {
	// 	_output = ITOA(ERR_NICKNAMEINUSE)  + '\n';	// => should ignore the command
	// 	return ;
	// }
	
	_sender->setNickname(arg[1]);
}

void	Message::_user(std::vector<std::string> arg)
{
	if (arg.size() < 5)
	{
		_output = ITOA(ERR_NEEDMOREPARAMS) + '\n';	// => server should reject command
		return ;
	}
	if (_sender->isWelcomed())
	{
		_output = ITOA(ERR_ALREADYREGISTERED) + '\n';	// => attempt should fail
		return ;
	}
	
	std::vector<std::string>::iterator	it = ++arg.begin();
	/* username */
	_sender->setUsername(*it++);
	/* mode */
	_sender->setMode(*it++);
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
	
	_sender->welcome();
	_output += "001 Welcome to the <networkname> Network, " + _sender->getNickname() + "\n"; //<nick>[!<user>@<host>]
}

void		Message::_privmsg(std::vector<std::string> arg)
{
	std::cout << "	*Message class: PRIVMSG cmd detected*\n";
	(void)arg;
}

void		Message::_kick(std::vector<std::string> arg)
{
	std::cout << "	*Message class: KICK cmd detected*\n";
	(void)arg;
}

void		Message::_invite(std::vector<std::string> arg)
{
	std::cout << "	*Message class: INVITE cmd detected*\n";
	(void)arg;
}

void		Message::_topic(std::vector<std::string> arg)
{
	std::cout << "	*Message class: TOPIC cmd detected*\n";
	(void)arg;
}

void		Message::_mode(std::vector<std::string> arg)
{
	std::cout << "	*Message class: MODE cmd detected*\n";
	try
	{
		User	target = _server->getUser(arg[1]);
		if (target.getNickname().compare(_sender->getNickname()))
			throw std::invalid_argument(ITOA(ERR_USERSDONTMATCH));
		if (arg.size() < 3)
			throw std::invalid_argument(ITOA(RPL_UMODEIS) + " " + target.getMode());
		// target.setMode(arg[2]);
	}
	catch(const std::invalid_argument& e)
	{
		_output += e.what();
		_output += "\n";
	}
	catch(const std::exception& e)
	{
		_output += ITOA(ERR_NOSUCHNICK) + "\n";
	}
}

void		Message::_ping(std::vector<std::string> arg)
{
	std::cout << "	*Message class: PING cmd detected*\n";
	if (arg.size() < 2)
	{
		_output = ITOA(ERR_NEEDMOREPARAMS) + '\n';
		return ;
	}
	_output = "PONG " + arg[1] + '\n';
}
