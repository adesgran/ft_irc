/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/05 14:14:15 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Message.hpp>

Message::Message(void)
{
	_cmdMap["NICK"]		= &Message::_nick;
	_cmdMap["USER"]		= &Message::_user;
	_cmdMap["JOIN"]		= &Message::_join;
	_cmdMap["PRIVMSG"]	= &Message::_privmsg;
	_cmdMap["KICK"]		= &Message::_kick;
	_cmdMap["INVITE"]	= &Message::_invite;
	_cmdMap["TOPIC"]	= &Message::_topic;
	_cmdMap["MODE"]		= &Message::_mode;
	_cmdMap["PING"]		= &Message::_ping;
	_cmdMap["WHOIS"]	= &Message::_whois;
}

Message::Message(User *sender): _sender(sender)
{
	_cmdMap["NICK"]		= &Message::_nick;
	_cmdMap["USER"]		= &Message::_user;
	_cmdMap["JOIN"]		= &Message::_join;
	_cmdMap["PRIVMSG"]	= &Message::_privmsg;
	_cmdMap["KICK"]		= &Message::_kick;
	_cmdMap["INVITE"]	= &Message::_invite;
	_cmdMap["TOPIC"]	= &Message::_topic;
	_cmdMap["MODE"]		= &Message::_mode;
	_cmdMap["PING"]		= &Message::_ping;
	_cmdMap["WHOIS"]	= &Message::_whois;
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

void		Message::setInputMsg(const std::string &input_buffer, Server *server)
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
void	Message::_parseInput(const std::vector<std::string> &input_lines)
{
	for (std::vector<std::string>::const_iterator	line = input_lines.begin();
			line != input_lines.end();
			line++)
	{
		std::stringstream	ss(*line);
		std::string			cmd_name, cmd_arg;
		std::getline(ss, cmd_name, ' ');
		std::getline(ss, cmd_arg);

		std::map<std::string, cmdFunction>::const_iterator cmd_ft;
		cmd_ft = _cmdMap.find(cmd_name);
		if (cmd_ft != _cmdMap.end())
		{
			(*this.*cmd_ft->second)(cmd_arg);
		}
		// _cmdMap[cmd_name](cmd_arg); cette ligne ne marche malheureusement pas :'(
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
		res.push_back(tmp);
		if (pos != std::string::npos)
			i = pos + sep.size();
		else
			i += tmp.size();;
	}
	return (res);
}

void	Message::appendOutputMsg(const std::string &err_code, const std::string &arg)
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

void	Message::_nick(const std::string &arg)
{
	std::cout << "	*Message class: NICK cmd detected*\n";
	if (arg.empty())
	{
		appendOutputMsg(ERR_NONICKNAMEGIVEN); // => should ignore the command
		return ;
	}
	if (arg.find_first_of("#: \t\n\r\v\f") != std::string::npos)
	{
		appendOutputMsg(ERR_ERRONEUSNICKNAME);	// => should ignore the command
		return ;
	}
	if (_server->isUser(arg))
	{
		appendOutputMsg(ERR_NICKNAMEINUSE);	// => should ignore the command
		return ;
	}
	_sender->setNickname(arg);
	
	_welcomeNewUser();
}

void	Message::_user(const std::string &arg)
{
	std::cout << "	*Message class: USER cmd detected*\n";

	if (_sender->isWelcomed())
	{
		appendOutputMsg(ERR_ALREADYREGISTERED);	// => attempt should fail
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
		appendOutputMsg(ERR_NEEDMOREPARAMS);	// => server should reject command
	}
}

void	Message::_join(const std::string &arg)
{
	std::cout << "	*Message class: JOIN cmd detected*\n";
	if (arg.empty())
	{
		appendOutputMsg(ERR_NEEDMOREPARAMS);	// => server should reject command
		return ;
	}
	// if (!arg.compare("0"))
	// {
	// 	// client leaves all joined channel, equivalent to the PART command"
	// }
	//ERR_NOSUCHCHANNEL
	//ERR_TOOMANYTARGETS
	//ERR_BANNEDFROMCHAN
	//ERR_BADCHANMASK
	//ERR_TOOMANYCHANNELS
	//ERR_UNAVAILRESOURCE
	
	std::vector<std::string>	tmp = _split(arg, " ");
	std::vector<std::string>	chan_name = _split (tmp[0], ","), key;
	if (tmp.size() > 1)
		key = _split(tmp[1], ",");
	
	size_t	i = 0;
	while (i < chan_name.size())
	{
		// if(!_server->isChannel(chan_name[i]))
		// _sender devient operator du channel
		Channel	&channel = _server->getChannel(chan_name[i]);
		if (channel.getActiveModes().find('k') == std::string::npos
			|| (channel.getActiveModes().find('k') != std::string::npos && !channel.getKey().compare(key[i])))
		{
			std::vector<User *> users = channel.getUsers();

			for ( std::vector<User *>::iterator it = users.begin();
					it != users.end();
					it++)
			{
				(*it)->getMessage()->_output << USERTAG(_sender) << " JOIN " << chan_name[i] << '\n';
			}
			channel.addUser(_sender);
			
			_output << USERTAG(_sender) << " JOIN " << arg << "\n";
			appendOutputMsg(RPL_TOPIC, channel.getTopic());
			_output << USERTAG(_sender) << " " << channel.status << " " << chan_name[i] << " :";
			for ( std::vector<User *>::iterator it = users.begin();
					it != users.end();
					it++)
				(*it)->getMessage()->_output << (*it)->getNickname() << " ";
			_output << _sender->getNickname() << "\n";
			_output << RPL_ENDOFNAMES << std::endl;
		}
		else
		{
			_output << ERR_BADCHANNELKEY << " " << chan_name[i] << "\n";
		}
		i++;
	}	
}

void		Message::_privmsg(const std::string &arg)
{
	std::cout << "	*Message class: PRIVMSG cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			target_name, text_to_send;
	if (!std::getline(ss, target_name, ' '))
	{
		appendOutputMsg(ERR_NORECIPIENT);
		return ;
	}
	if (!std::getline(ss, text_to_send))
	{
		appendOutputMsg(ERR_NOTEXTTOSEND);
		return ;
	}
	
	if (target_name[0] == '#')
	{
		// Channels with the moderated mode active may block messages from certain users
		// Other channel modes may affect the delivery of the message or cause the message to be modified before delivery, and these modes are defined by the server software and configuration being used
		std::vector<User *> chan_users = _server->getChannel(target_name).getUsers();

		for ( std::vector<User *>::iterator it = chan_users.begin();
				it != chan_users.end();
				it++)
			(*it)->getMessage()->_output << target_name << text_to_send << '\n';
	}
	else try
	{
		User	&target = _server->getUser(target_name);
		if (target.getActiveModes().find('a') != std::string::npos)
			appendOutputMsg(RPL_AWAY, "");
		target.getMessage()->_output << _sender->getNickname() << text_to_send << '\n';
	}
	catch(const std::exception& e)
	{
		appendOutputMsg(ERR_NOSUCHNICK);
	}
}

void		Message::_kick(const std::string &arg)
{
	std::cout << "	*Message class: KICK cmd detected*\n";
	std::cout << "	*arg: " << arg << "*\n";
	std::stringstream	ss(arg);
	std::string			chan_name, comment, target_list;
	std::vector<std::string>	targets;
	try
	{
		if (!std::getline(ss, chan_name, ' '))
			throw std::invalid_argument(ERR_NEEDMOREPARAMS);
		if (!std::getline(ss, target_list, ' '))
			throw std::invalid_argument(ERR_NEEDMOREPARAMS);
		targets = _split(target_list, ",");
		if (!std::getline(ss, comment) || (comment.size() <= 1 && !comment.compare(":")))
			comment = ":You have been kicked from the " + chan_name + " channel.\n";
		if (!_server->isChannel(chan_name))
			throw std::invalid_argument(ERR_NOSUCHCHANNEL);
		Channel	&channel = _server->getChannel(chan_name);
		if (!channel.isUserOnChannel(_sender->getNickname()))
			throw std::invalid_argument(ERR_NOTONCHANNEL);
		// if (_sender n'est pas un operator)
		// throw std::invalid_argument(ERR_CHANOPRIVSNEEDED);
		for (std::vector<std::string>::iterator it = targets.begin();
				it != targets.end();
				it++)
		{
			if (!channel.isUserOnChannel(*it))
				appendOutputMsg(ERR_USERNOTINCHANNEL);
			else
			{
				User	*rm_user = &_server->getUser(*it);
				channel.removeUser(rm_user);
				rm_user->getMessage()->_output << "KICK " << comment << "\n";
			}
		}
	}
	catch(const std::exception& e)
	{
		appendOutputMsg(e.what());
	}
}

void		Message::_invite(const std::string &arg)
{
	std::cout << "	*Message class: INVITE cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			target_nickname, target_chan;
	try
	{
		std::cout << "ICI\n";
		if (!std::getline(ss, target_nickname, ' ') || !std::getline(ss, target_chan, ' '))
			throw std::invalid_argument(ERR_NEEDMOREPARAMS);
		if (!_server->isUser(target_nickname))
			throw std::invalid_argument(ERR_NOSUCHNICK); // Pas indiqué dans la doc ??
		if (!_server->isChannel(target_chan))
			throw std::invalid_argument(ERR_NOSUCHCHANNEL);
		Channel &channel = _server->getChannel(target_chan);
		if (!channel.isUserOnChannel(target_nickname))
			throw std::invalid_argument(ERR_NOTONCHANNEL);
		channel.addUser(&_server->getUser(target_nickname), _sender);
		appendOutputMsg(RPL_INVITING);
		_server->getUser(target_nickname).getMessage()->_output << ":" << USERTAG(_sender) << " INVITE " << target_nickname << " " << target_chan << "\n";
	}
	catch(const std::exception& e)
	{
		appendOutputMsg(e.what());
	}
}

void		Message::_topic(const std::string &arg)
{
	std::cout << "	*Message class: TOPIC cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			target, new_topic;

	try
	{
		if (!std::getline(ss, target, ' '))
			throw std::invalid_argument(ERR_NEEDMOREPARAMS);
		if (!_server->isChannel(target))
			throw std::invalid_argument(ERR_NOSUCHCHANNEL);
		Channel	&channel = _server->getChannel(target);
		if (!channel.isUserOnChannel(_sender->getNickname()))
			throw std::invalid_argument(ERR_NOTONCHANNEL);
		if (!std::getline(ss, new_topic))
		{
			if (channel.getTopic().empty())
				appendOutputMsg(RPL_NOTOPIC);
			else
				appendOutputMsg(RPL_TOPIC, channel.getTopic());
			return ;
		}
		if (new_topic[0] == ':')
			new_topic.erase(0, 1);
		channel.setTopic(_sender, new_topic);
		std::vector<User *> chan_users = channel.getUsers();

		for ( std::vector<User *>::iterator it = chan_users.begin();
				it != chan_users.end();
				it++)
			(*it)->getMessage()->_output << "TOPIC " << target << " :" << new_topic << '\n';
	}
	catch(const std::exception& e)
	{
		appendOutputMsg(e.what());
	}
	
}

void		Message::_mode(const std::string &arg)
{
	std::cout << "	*Message class: MODE cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			target_name, mode_list, mode_arg;
	std::getline(ss, target_name, ' ');

	try
	{
		if (target_name[0] == '#')
		{
			if (!_server->isChannel(target_name))
				throw std::invalid_argument(ERR_NOSUCHCHANNEL);
			Channel	&channel = _server->getChannel(target_name);
			if (!std::getline(ss, mode_list, ' '))
			{
				appendOutputMsg(RPL_CHANNELMODEIS, channel.getActiveModes());
				return ;
			}
			// if sender != chanops
			// ERR_CHANOPRIVSNEEDED
			if (!std::getline(ss, mode_list, ' '))
			{
				appendOutputMsg(RPL_UMODEIS, channel.getActiveModes());
				return ;
			}
			std::getline(ss, mode_arg);
			channel.setModes(mode_list, mode_arg);
		}
		else
		{
			if (!_server->isUser(target_name))
				throw std::invalid_argument(ERR_NOSUCHNICK);
			if (target_name.compare(_sender->getNickname()))
				throw std::invalid_argument(ERR_USERSDONTMATCH);
			if (!std::getline(ss, mode_list, ' '))
			{
				appendOutputMsg(RPL_UMODEIS, _sender->getActiveModes());
				return ;
			}
			_sender->setModes(mode_list);
			_output << "MODE " << _sender->getNickname() << " " << _sender->getActiveModes() << "\n";
		}
	}
	catch(const std::exception& e)
	{
		appendOutputMsg(e.what());
	}
	
}

void		Message::_ping(const std::string &arg)
{
	std::cout << "	*Message class: PING cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			ret, mode_list;
	std::getline(ss, ret, ' ');
	if (ret.empty())
	{
		appendOutputMsg(ERR_NEEDMOREPARAMS);
		return ;
	}
	_output << "PONG " << ret << "\n";
}

void	Message::_whois(const std::string &arg)
{
	std::cout << "	*Message class: WHOIS cmd detected*\n";
	std::stringstream	ss(arg);
	std::string			target_name, mode_list;
	std::getline(ss, target_name, ' ');
	// check aussi si target = nom du server
	// sinon => ERR_NOSUCHSERVER 
	if (_server->isUser(target_name))
	{
		appendOutputMsg(ERR_NOSUCHNICK);
		return ;
	}
	appendOutputMsg(RPL_ENDOFWHOIS);
}
