/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/03 14:17:50 by mchassig         ###   ########.fr       */
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
		std::stringstream	ss(*line);
		std::string			cmd_name, cmd_arg;
		std::getline(ss, cmd_name, ' ');
		std::getline(ss, cmd_arg);
		switch (_cmdMap[cmd_name])
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
		if (tmp.size() >= 1)
			res.push_back(tmp);
		if (pos != std::string::npos)
			i = pos + sep.size();
		else
			i += tmp.size();;
	}
	return (res);
}

void	Message::_appendOutputMsg(std::string err_code, std::string arg)
{
	_output << err_code;
	if (arg.size())
	{
		_output << " " << arg;
	}
	_output << "\n";
}


// IRC commands -----------------------------------------
void	Message::_welcomeNewUser()
{
	if (!_sender->isWelcomed() && !_sender->getNickname().empty()
		&& !_sender->getUsername().empty())
	{
		_sender->welcome();
		_output << ":" << std::string(SERVER_ADDRESS) << " " << RPL_WELCOME << " " << _sender->getNickname() 
		<< " :Welcome to the <networkname> Network " 
		<< USERTAG(_sender)
		<< "\n";
	}
}

void	Message::_nick(std::string arg)
{
	std::cout << "	*Message class: NICK cmd detected*\n";
	if (arg.empty())
	{
		_appendOutputMsg(ERR_NONICKNAMEGIVEN); // => should ignore the command
		return ;
	}
	if (arg.find_first_of("#: \t\n\r\v\f") != std::string::npos)
	{
		_appendOutputMsg(ERR_ERRONEUSNICKNAME);	// => should ignore the command
		return ;
	}
	if (_server->isUser(arg))
	{
		_appendOutputMsg(ERR_NICKNAMEINUSE);	// => should ignore the command
		return ;
	}
	_sender->setNickname(arg);
	
	_welcomeNewUser();
}

void	Message::_user(std::string arg)
{
	std::cout << "	*Message class: USER cmd detected*\n";

	if (_sender->isWelcomed())
	{
		_appendOutputMsg(ERR_ALREADYREGISTERED);	// => attempt should fail
		return ;
	}

	std::stringstream	ss(arg);
	std::string			username, mode, unused, realName;

	try
	{
		if (!std::getline(ss, username, ' '))
			throw std::exception();
		_sender->setUsername(username);
		if (!std::getline(ss, mode, ' '))
			throw std::exception();
		(void)mode;
		if (!std::getline(ss, unused, ' '))
			throw std::exception();
		(void)unused;
		if (!std::getline(ss, realName, ' '))
			throw std::exception();
		if (realName[0] == ':')
			realName.erase(0, 1);
		_sender->setRealname(realName);
		
		_welcomeNewUser();
	}
	catch(const std::exception& e)
	{
		_appendOutputMsg(ERR_NEEDMOREPARAMS);	// => server should reject command
	}
}

void	Message::_join(std::string arg)
{
	std::cout << "	*Message class: JOIN cmd detected*\n";
	if (arg.empty())
	{
		_appendOutputMsg(ERR_NEEDMOREPARAMS);	// => server should reject command
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
	
	std::vector<User *> users = _server->getChannel(arg).getUsers();

	 for ( std::vector<User *>::iterator it = users.begin();
			 it != users.end();
			 it++)
		 (*it)->getMessage()->_output << USERTAG(_sender) << " JOIN " << arg << '\n';

	 _server->getChannel(arg).addUser(_sender);
	
	_output << USERTAG(_sender) << " JOIN " << arg[1] << "\n";
	
}

void		Message::_privmsg(std::string arg)
{
	std::cout << "	*Message class: PRIVMSG cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			target_name, text_to_send;
	if (!std::getline(ss, target_name, ' '))
	{
		_appendOutputMsg(ERR_NORECIPIENT);
		return ;
	}
	if (!std::getline(ss, text_to_send))
	{
		_appendOutputMsg(ERR_NOTEXTTOSEND);
		return ;
	}
	
	if (target_name.find('#') != std::string::npos)
	{
		// if server is banned from channel => command silently fails
		// Channels with the moderated mode active may block messages from certain users
		// Other channel modes may affect the delivery of the message or cause the message to be modified before delivery, and these modes are defined by the server software and configuration being used
	
		// msg couldn't be delivered : ERR_CANNOTSENDTOCHAN
		std::vector<User *> chan_users = _server->getChannel(target_name).getUsers();

		for ( std::vector<User *>::iterator it = chan_users.begin();
				it != chan_users.end();
				it++)
			(*it)->getMessage()->_output << target_name << text_to_send << '\n';
	}
	else try
	{
		User	&target = _server->getUser(target_name);
		if (target.getMode().find('a') != std::string::npos)
			_appendOutputMsg(RPL_AWAY, "");
		target.getMessage()->_output << _sender->getNickname() << text_to_send << '\n';
	}
	catch(const std::exception& e)
	{
		_appendOutputMsg(ERR_NOSUCHNICK);
	}
	}

void		Message::_kick(std::string arg)
{
	std::cout << "	*Message class: KICK cmd detected*\n";
	(void)arg;
}

void		Message::_invite(std::string arg)
{
	std::cout << "	*Message class: INVITE cmd detected*\n";
	(void)arg;
}

void		Message::_topic(std::string arg)
{
	std::cout << "	*Message class: TOPIC cmd detected*\n";
	(void)arg;
}

void		Message::_mode(std::string arg)
{
	std::cout << "	*Message class: MODE cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			target_name, mode_list;
	std::getline(ss, target_name, ' ');

	if (!_server->isUser(target_name))
	{
		_appendOutputMsg(ERR_NOSUCHNICK);
		return ;
	}
	User	&target = _server->getUser(target_name);
	if (target.getNickname().compare(_sender->getNickname()))
	{
		_appendOutputMsg(ERR_USERSDONTMATCH);
		return ;
	}
	if (arg.size() < 3)
	{
		_appendOutputMsg(RPL_UMODEIS, _sender->getMode());
		return ;
	}
	char	op = 0;
	bool	err = false;
	std::getline(ss, mode_list, ' ');

	for (size_t i = 0; i < mode_list.size(); i++)
	{
		if (mode_list[i] == '+' || mode_list[i] == '-')
		{
			op = mode_list[i];
		}
		else if (_server->isModeImplemented(mode_list[i]) && op == '+'
				&& mode_list[i] != 'o' && mode_list[i] != 'O')
		{
			_sender->addMode(mode_list[i]);
		}
		else if (_server->isModeImplemented(mode_list[i]) && op == '-'
				&& mode_list[i] != 'r')
		{
			_sender->removeMode(mode_list[i]);
		}
		else if (mode_list[i] != 'o' && mode_list[i] != 'O' && mode_list[i] != 'r')
		{
			err = true;
		}
	}
	_output << "MODE " << _sender->getNickname() << " " << _sender->getMode() << "\n";
	if (err)
	{
		_appendOutputMsg(ERR_UMODEUNKNOWNFLAG, _sender->getMode());
		return ;
	}
}

void		Message::_ping(std::string arg)
{
	std::cout << "	*Message class: PING cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			ret, mode_list;
	std::getline(ss, ret, ' ');
	if (ret.empty())
	{
		_appendOutputMsg(ERR_NEEDMOREPARAMS);
		return ;
	}
	_output << "PONG " << ret << "\n";
}

void	Message::_whois(std::string arg)
{
	std::cout << "	*Message class: WHOIS cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			target_name, mode_list;
	std::getline(ss, target_name, ' ');
	// check aussi si target = nom du server
	// sinon => ERR_NOSUCHSERVER 
	if (_server->isUser(target_name))
	{
		_appendOutputMsg(ERR_NOSUCHNICK);
		return ;
	}
	_appendOutputMsg(RPL_ENDOFWHOIS);
}
