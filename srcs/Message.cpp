/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/01 10:14:06 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Message.hpp>

Message::Message(void)
{
	_cmdMap["NICK"] = NICK;
	_cmdMap["USER"] = USER;
	_cmdMap["JOIN"] = JOIN;
	_cmdMap["PRIVMSG"] = PRIVMSG;
	_cmdMap["KICK"] = KICK;
	_cmdMap["INVITE"] = INVITE;
	_cmdMap["TOPIC"] = TOPIC;
	_cmdMap["MODE"] = MODE;
	_cmdMap["PING"] = PING;
	_cmdMap["WHOIS"] = WHOIS;
	_cmdMap["CAP"] = CAP;

}

Message::Message(User *sender): _sender(sender)
{
	_cmdMap["NICK"] = NICK;
	_cmdMap["USER"] = USER;
	_cmdMap["JOIN"] = JOIN;
	_cmdMap["PRIVMSG"] = PRIVMSG;
	_cmdMap["KICK"] = KICK;
	_cmdMap["INVITE"] = INVITE;
	_cmdMap["TOPIC"] = TOPIC;
	_cmdMap["MODE"] = MODE;
	_cmdMap["PING"] = PING;
	_cmdMap["WHOIS"] = WHOIS;
	_cmdMap["CAP"] = CAP;

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
	this->_output << message._output.rdbuf();
	return (*this);
}

// void	Message::read( std::string message )
// {
// 	std::cout << message << "\n";
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

std::string	Message::getOutputMsg() 
{
	std::string res = _output.str();
	_output.str("");
	return (std::string(res));
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
			case JOIN:
				_join(cmd_arg);
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
			case WHOIS:
				_whois(cmd_arg);
				break;
			case CAP:
				break;
			default:
				break;
		}
	}
}

std::vector<std::string>	Message::_split(const std::string &str, const std::string &sep) const
{
	std::vector<std::string> res;
    size_t	pos = 0;
	size_t	i = 0;
    while (str[i])
	{
        pos = str.find(sep, i);
		std::string	tmp = str.substr(i, pos - i);
		if (tmp.size() > 1)
        	res.push_back(tmp);
		if (pos != std::string::npos)
        	i = pos + sep.size();
		else
			i += tmp.size();;
   }
    return (res);
}

// IRC commands -----------------------------------------
void	Message::_nick(std::vector<std::string> arg)
{
	std::cout << "	*Message class: NICK cmd detected*\n";

	if (arg.size() < 2)
	{
		_output << ITOA(ERR_NONICKNAMEGIVEN) << "\n"; //	=> should ignore the command
		return ;
	}
	if (arg[1].find_first_not_of("_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[]{}\\|") != std::string::npos)
	{
		_output << ITOA(ERR_ERRONEUSNICKNAME) << "\n";	// => should ignore the command
		return ;
	}
	try
	{
		_server->getUser(arg[1]);
		_output << ITOA(ERR_NICKNAMEINUSE)  << "\n";	// => should ignore the command
	}
	catch(const std::exception& e)
	{
		_sender->setNickname(arg[1]);
	}
	
}

void	Message::_user(std::vector<std::string> arg)
{
	std::cout << "	*Message class: USER cmd detected*\n";

	if (arg.size() < 5)
	{
		_output << ITOA(ERR_NEEDMOREPARAMS) << "\n";	// => server should reject command
		return ;
	}
	if (_sender->isWelcomed())
	{
		_output << ITOA(ERR_ALREADYREGISTERED) << "\n";	// => attempt should fail
		return ;
	}
	
	std::vector<std::string>::iterator	it = ++arg.begin();
	/* username */
	_sender->setUsername(*it++);
	/* mode */
	it++;
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
	_output << ":" << std::string(SERVER_ADDRESS) << " 001 " << _sender->getNickname() 
		<< " :Welcome to the <networkname> Network " 
		<< _sender->getNickname() << "!" << _sender->getUsername() << "@localhost"
		<< "\n"; //<nick>[!<user>@<host>]
}

void	Message::_join(std::vector<std::string> arg)
{
	std::cout << "	*Message class: JOIN cmd detected*\n";
	if (arg.size() != 2)
	{
		_output << ERR_NEEDMOREPARAMS << "\n";	// => server should reject command
		return ;
	}
	//ERR_INVITEONLYCHAN
	//ERR_CHANNELISFULL
	//ERR_NOSUCHCHANNEL
	//ERR_TOOMANYTARGETS
	//ERR_BANNEDFROMCHAN
	//ERR_BADCHANNELKEY
	//ERR_BADCHANMASK
	//ERR_TOOMANYCHANNELS
	//ERR_UNAVAILRESOURCE
	//
	
	std::vector<User *> users = _server->getChannel(arg[1]).getUsers();

	 for ( std::vector<User *>::iterator it = users.begin();
			 it != users.end();
			 it++)
		 (*it)->getMessage()->_output << USERTAG(_sender) << " JOIN " << arg[1] << '\n';

	 _server->getChannel(arg[1]).addUser(_sender);
	
	
	_output << USERTAG(_sender) << " JOIN " << arg[1] << "\n";
	
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
		User	&target = _server->getUser(arg[1]);
		if (target.getNickname().compare(_sender->getNickname()))
			throw std::invalid_argument(ITOA(ERR_USERSDONTMATCH));
		if (arg.size() < 3)
			throw std::invalid_argument(ITOA(RPL_UMODEIS) + " " + _sender->getMode());
		char	op = 0;
		bool	err = false;
		
		for (size_t i = 0; i < arg[2].size(); i++)
		{
			if (arg[2][i] == '+' || arg[2][i] == '-')
			{
				op = arg[2][i];
			}
			else if (_server->isModeImplemented(arg[2][i]) && op == '+'
					&& arg[2][i] != 'o' && arg[2][i] != 'O')
			{
				_sender->addMode(arg[2][i]);
			}
			else if (_server->isModeImplemented(arg[2][i]) && op == '-'
					&& arg[2][i] != 'r')
			{
				_sender->removeMode(arg[2][i]);
			}
			else if (arg[2][i] != 'o' && arg[2][i] != 'O' && arg[2][i] != 'r')
			{
				err = true;
			}
		}
		_output << "MODE " << _sender->getNickname() << " " << _sender->getMode() << "\n";
		if (err)
			throw std::invalid_argument(ITOA(ERR_UMODEUNKNOWNFLAG));
	}
	catch(const std::invalid_argument& e)
	{
		_output << e.what() << "\n";
	}
	catch(const std::exception& e)
	{
		_output << ITOA(ERR_NOSUCHNICK) << "\n";
	}
}

void		Message::_ping(std::vector<std::string> arg)
{
	std::cout << "	*Message class: PING cmd detected*\n";
	if (arg.size() < 2)
	{
		_output << ITOA(ERR_NEEDMOREPARAMS) << "\n";
		return ;
	}
	_output << "PONG " << arg[1] << "\n";
}

void	Message::_whois(std::vector<std::string> arg)
{
	std::cout << "	*Message class: WHOIS cmd detected*\n";
	try
	{
		// check aussi si target = nom du server
		// sinon => ERR_NOSUCHSERVER 
		_server->getUser(arg[1]);
		_output << ITOA(RPL_ENDOFWHOIS) << "\n";
	}
	catch(const std::exception& e)
	{
		_output << ITOA(ERR_NOSUCHNICK) << "\n";
	}
	
	(void)arg;
}
