/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/10 15:34:48 by adesgran         ###   ########.fr       */
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
	this->_cmdMap = message._cmdMap;
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
	_parseInput(_split(_input, CRLF));
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

void	Message::addNumericMsg(const std::string code, const std::string arg)
{
	_output << ':' << USERTAG(_sender);
	_output << " " << code;
	_output << " " << _sender->getNickname();
	if (!arg.empty())
		_output << " " << arg;
	_output << CRLF;
}

void	Message::addMsg(const User *source, const std::string cmd, const std::string target, const std::string arg)
{
	_output << ':' << USERTAG(source);
	_output << " " << cmd;
	_output << " " << target;
	if (!arg.empty())
		_output << " " << arg;
	_output << CRLF;
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

		if (_cmdMap.find(cmd_name) != _cmdMap.end())
			(this->*_cmdMap[cmd_name])(cmd_arg);
		else if (!cmd_name.compare("PART"))
		{
			std::vector<Channel *>	chans = _server->getChannels();
			std::cout << "	*number of channels: " << chans.size() << std::endl;
			for (std::vector<Channel *>::iterator c = chans.begin();
					c != chans.end(); c ++)
			{
				std::cout << "	| " << (*c)->getName();
				std::vector<User *>	users = (*c)->getUsers();
				std::cout << " (" << users.size() << " users):";
				for (std::vector<User *>::iterator u = users.begin();
						u != users.end(); u ++)
				{
					std::cout << " " << (*u)->getNickname();
				}
				std::cout << std::endl;
			}
		}
		else if (!cmd_name.empty())
			addNumericMsg(ERR_UNKNOWNCOMMAND, cmd_name + " :Unknown command");
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
		<< CRLF;
	}
}

void	Message::_nick(const std::string &arg)
{
	try
	{
		if (arg.empty())
			throw NumericReply(ERR_NONICKNAMEGIVEN, ":No nickname given");
		if (arg.find_first_of("#:@ \t\n\r\v\f") != std::string::npos)
			throw NumericReply(ERR_ERRONEUSNICKNAME, arg + " :Erroneus nickname");
		if (_server->isUser(arg))
			throw NumericReply(ERR_NICKNAMEINUSE, arg + " :Nickname is already in use");
		_sender->setNickname(arg);		
		_welcomeNewUser();
	}
	catch(const NumericReply& e)
	{
		addNumericMsg(e.what(), e.param());
	}
}

void	Message::_user(const std::string &arg)
{
	std::stringstream	ss(arg);
	std::string			username, mode, unused, realName;
	try
	{
		if (_sender->isWelcomed())
			throw NumericReply(ERR_ALREADYREGISTERED, ":You may not reregister");
		if (!std::getline(ss, username, ' ') || !std::getline(ss, mode, ' ') \
				|| !std::getline(ss, unused, ' ') || !std::getline(ss, realName))
			throw NumericReply(ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		_sender->setUsername(username);
		(void)mode;
		(void)unused;
		if (realName[0] == ':')
			realName.erase(0, 1);
		_sender->setRealname(realName);
		
		_welcomeNewUser();
	}
	catch(const NumericReply& e)
	{
		addNumericMsg(e.what(), e.param());
	}
}

void	Message::_join(const std::string &arg)
{
	if (arg.empty())
	{
		addNumericMsg(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
		return ;
	}
	// if (!arg.compare("0"))
	// {
	// 	// client leaves all joined channel, equivalent to the PART command"
	// }
	//ERR_NOSUCHCHANNEL
	//ERR_TOOMANYTARGETS
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
		try
		{
			Channel *channel;
			if (!_server->isChannel(chan_name[i]))
			{
				_server->addChannel(new Channel(chan_name[i], _sender));
				channel = &_server->getChannel(chan_name[i]);
			}
			else
			{
				channel = &_server->getChannel(chan_name[i]);
				channel->addUser(_sender);
			}
			std::vector<User *> users = channel->getUsers();
			std::string			user_list;
			for ( std::vector<User *>::iterator it = users.begin();
					it != users.end();
					it++)
			{
				(*it)->getMessage()->addMsg(_sender, "JOIN", chan_name[i]);
				user_list += (*it)->getNickname() + " ";
			}
			addNumericMsg(RPL_TOPIC, channel->getTopic());
			addNumericMsg(RPL_NAMREPLY, ":" + user_list);
			addNumericMsg(RPL_ENDOFNAMES);
		}
		catch(const NumericReply& e)
		{
			addNumericMsg(e.what(), e.param());
		}
		i++;
	}
}

void	Message::_privmsg(const std::string &arg)
{
	std::stringstream	ss(arg);
	std::string			target_name, text_to_send;

	try
	{
		if (!std::getline(ss, target_name, ' '))
			throw NumericReply(ERR_NORECIPIENT, " :No recipient given PRIVMSG");
		if (!std::getline(ss, text_to_send))
			throw NumericReply(ERR_NOTEXTTOSEND, " :No text to send");
		if (target_name[0] == '#')
		{
			std::vector<User *> chan_users = _server->getChannel(target_name).getUsers();
			for ( std::vector<User *>::iterator it = chan_users.begin();
					it != chan_users.end();
					it++)
			{
				if (*it != _sender)
					(*it)->getMessage()->addMsg(_sender, "PRIVMSG", target_name, text_to_send);
			}
		}
		else
		{
			User	&target = _server->getUser(target_name);
			if (target.getActiveModes().find('a') != std::string::npos)
				addNumericMsg(RPL_AWAY, target_name);
			target.getMessage()->addMsg(_sender, "PRIVMSG", target_name, text_to_send);
			//target.getMessage()->_output << _sender->getNickname() << ' ' << text_to_send << CRLF;
		}
	}
	catch(const NumericReply& e)
	{
		addNumericMsg(e.what(), e.param());
	}
	catch(const std::exception& e)
	{
		addNumericMsg(ERR_NOSUCHNICK, target_name + " :No such nick/channel");
	}
}

void	Message::_kick(const std::string &arg)
{
	std::stringstream	ss(arg);
	std::string			chan_name, comment, target_list;
	std::vector<std::string>	targets;
	try
	{
		if (!std::getline(ss, chan_name, ' ') || !std::getline(ss, target_list, ' '))
			throw NumericReply(ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
		targets = _split(target_list, ",");
		if (!std::getline(ss, comment) || (comment.size() <= 1 && !comment.compare(":")))
			comment = ":You have been kicked from the " + chan_name + " channel" + CRLF;
		Channel	&channel = _server->getChannel(chan_name);
		if (!channel.isUserOnChannel(_sender->getNickname()))
			throw NumericReply(ERR_NOTONCHANNEL, chan_name + " :You're not on that channel");
		if (!channel.isChanop(_sender->getNickname()))
			throw NumericReply(ERR_CHANOPRIVSNEEDED,chan_name + " :You're not channel operator");
		std::vector<User *> chan_users = channel.getUsers();
		for (std::vector<std::string>::iterator it = targets.begin();
				it != targets.end();
				it++)
		{
			if (!channel.isUserOnChannel(*it))
				addNumericMsg(ERR_USERNOTINCHANNEL, *it + " " + channel.getName() + " :They aren't on that channel");
			else
			{
				User	*rm_user = &_server->getUser(*it);
				channel.removeUser(rm_user);
				rm_user->getMessage()->addMsg(_sender, "KICK", rm_user->getNickname(), comment);
				// rm_user->getMessage()->_output << USERTAG(_sender) << " KICK " << comment << CRLF;
			}
			for ( std::vector<User *>::iterator it = chan_users.begin();
					it != chan_users.end();
					it++)
				(*it)->getMessage()->addMsg(_sender, "KICK", chan_name, (*it)->getNickname());
				// (*it)->getMessage()->_output << USERTAG(_sender) << " KICK " << chan_name << " " << *it << CRLF;
		}
	}
	catch(const NumericReply& e)
	{
		addNumericMsg(e.what(), e.param());
	}
}

void	Message::_invite(const std::string &arg)
{
	std::stringstream	ss(arg);
	std::string			target_nickname, target_chan;
	try
	{
		if (!std::getline(ss, target_nickname, ' ') || !std::getline(ss, target_chan, ' '))
			throw NumericReply(ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
		if (!_server->isUser(target_nickname))
			throw NumericReply(ERR_NOSUCHNICK, target_nickname + " :No such nick/channel");
		Channel &channel = _server->getChannel(target_chan);
		if (!channel.isUserOnChannel(_sender->getNickname()))
			throw NumericReply(ERR_NOTONCHANNEL, target_chan + " :You're not on that channel");
		channel.addUser(&_server->getUser(target_nickname), _sender);
		addNumericMsg(RPL_INVITING, target_nickname + " " + target_chan);
		_server->getUser(target_nickname).getMessage()->addMsg(_sender, "INVITE", target_nickname, target_chan);
		// _server->getUser(target_nickname).getMessage()->_output << ":" << USERTAG(_sender) << " INVITE " << target_nickname << " " << target_chan << CRLF;
	}
	catch (const NumericReply &e)
	{
		addNumericMsg(e.what(), e.param());
	}
}

void	Message::_topic(const std::string &arg)
{
	std::stringstream	ss(arg);
	std::string			target, new_topic;

	try
	{
		if (!std::getline(ss, target, ' '))
			throw NumericReply(ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		Channel	&channel = _server->getChannel(target);
		if (!channel.isUserOnChannel(_sender->getNickname()))
			throw NumericReply(ERR_NOTONCHANNEL, target + " :!!You're not on that channel"); // ERR optionnelle
		if (!std::getline(ss, new_topic))
		{
			if (channel.getTopic().empty())
				throw NumericReply(RPL_NOTOPIC, target + ":No topic is set");
			else
				throw NumericReply(RPL_TOPIC, target + channel.getTopic());
		}
		channel.setTopic(_sender, new_topic);
		std::vector<User *> chan_users = channel.getUsers();
		for ( std::vector<User *>::iterator it = chan_users.begin();
				it != chan_users.end();
				it++)
			(*it)->getMessage()->addMsg(_sender, "TOPIC", target, new_topic);
			// (*it)->getMessage()->_output << ":" << USERTAG(_sender) << " TOPIC " << target << " " << new_topic << CRLF;
	}
	catch (const NumericReply &e)
	{
		addNumericMsg(e.what(), e.param());
	}
}

void	Message::_mode(const std::string &arg)
{
	std::stringstream	ss(arg);
	std::string			target, modestring;
	std::getline(ss, target, ' ');
	try
	{
		if (target[0] == '#')
		{
			Channel	&channel = _server->getChannel(target);
			if (!std::getline(ss, modestring, ' '))
				throw NumericReply(RPL_CHANNELMODEIS, target + " " + channel.getActiveModes());
			else
			{
				if (!channel.isChanop(_sender->getNickname()))
					throw NumericReply(ERR_CHANOPRIVSNEEDED, target + " :You're not a channel operator");
				channel.setModes(_sender, modestring, ss);
				std::vector<User *> chan_users = channel.getUsers();
				for ( std::vector<User *>::iterator it = chan_users.begin();
						it != chan_users.end();
						it++)
					(*it)->getMessage()->_output << "MODE " << target << " :" << channel.getActiveModes() << CRLF;
			}
		}
		else
		{
			if (!_server->isUser(target))
				throw NumericReply(ERR_NOSUCHNICK, target + " :No such nick/channel");
			if (target.compare(_sender->getNickname()))
				throw NumericReply(ERR_USERSDONTMATCH, ":Cant change mode for other users");
			if (!std::getline(ss, modestring, ' '))
				throw NumericReply(RPL_UMODEIS, _sender->getActiveModes());
			else
			{
				_sender->setModes(modestring);
				addMsg(_sender, "MODE", target, _sender->getActiveModes());
				// _output << ":" << USERTAG(_sender) << " MODE " << target << " " << _sender->getActiveModes() << CRLF;	
			}
		}
	}
	catch (const NumericReply &e)
	{
		addNumericMsg(e.what(), e.param());
	}
}

void	Message::_ping(const std::string &arg)
{
	std::stringstream	ss(arg);
	std::string			ret, mode_list;
	std::getline(ss, ret, ' ');
	if (ret.empty())
	{
		addNumericMsg(ERR_NEEDMOREPARAMS, " PING :Not enough parameters");
		return ;
	}
	_output << "PONG " << ret << CRLF;
}

void	Message::_whois(const std::string &arg)
{
	std::stringstream	ss(arg);
	std::string			target_name, mode_list;
	std::getline(ss, target_name, ' ');
	// check aussi si target = nom du server
	// sinon => ERR_NOSUCHSERVER 
	if (!_server->isUser(target_name))
		addNumericMsg(ERR_NOSUCHNICK,target_name + " :No such nick/channel");
	else
		addNumericMsg(RPL_ENDOFWHOIS, target_name + " :End of /WHOIS list");
}
