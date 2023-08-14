/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/14 17:03:41 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Message.hpp>

Message::Message(void)
{
	_cmdMap["PASS"]		= &Message::_pass;
	_cmdMap["NICK"]		= &Message::_nick;
	_cmdMap["USER"]		= &Message::_user;
	_cmdMap["PING"]		= &Message::_pong;
	_cmdMap["JOIN"]		= &Message::_join;
	_cmdMap["TOPIC"]	= &Message::_topic;
	_cmdMap["INVITE"]	= &Message::_invite;
	_cmdMap["KICK"]		= &Message::_kick;
	_cmdMap["MODE"]		= &Message::_mode;
	_cmdMap["PRIVMSG"]	= &Message::_privmsg;
	_cmdMap["WHOIS"]	= &Message::_whois;
}

Message::Message(User *sender): _sender(sender)
{
	_cmdMap["PASS"]		= &Message::_pass;
	_cmdMap["NICK"]		= &Message::_nick;
	_cmdMap["USER"]		= &Message::_user;
	_cmdMap["PING"]		= &Message::_pong;
	_cmdMap["JOIN"]		= &Message::_join;
	_cmdMap["TOPIC"]	= &Message::_topic;
	_cmdMap["INVITE"]	= &Message::_invite;
	_cmdMap["KICK"]		= &Message::_kick;
	_cmdMap["MODE"]		= &Message::_mode;
	_cmdMap["PRIVMSG"]	= &Message::_privmsg;
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
		<< " :Welcome to the <networkname> Network " << USERTAG(_sender)
		<< CRLF;
	}
}

void	Message::_pass(const std::string &arg)
{
	try
	{
		if (arg.empty())
			throw NumericReply(ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
		if (_sender->authentificated)
			throw NumericReply(ERR_ALREADYREGISTERED, ":You may not reregister");
		if (arg.compare(_server->getPassword()))
			throw NumericReply(ERR_PASSWDMISMATCH, ":Password incorrect");
		_sender->authentificated = true;
	}
	catch(const NumericReply& e)
	{
		addNumericMsg(e.code(), e.what());
	}	
}

void	Message::_nick(const std::string &arg)
{
	if (!_sender->authentificated)
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}

	try
	{
		if (arg.empty())
			throw NumericReply(ERR_NONICKNAMEGIVEN, ":No nickname given");
		if (arg.find_first_of(",#:@ \t\n\r\v\f") != std::string::npos)
			throw NumericReply(ERR_ERRONEUSNICKNAME, arg + " :Erroneus nickname");
		if (!isEquals(arg, _sender->getNickname()) && _server->isUser(arg))
			throw NumericReply(ERR_NICKNAMEINUSE, arg + " :Nickname is already in use");
		_sender->setNickname(arg);
		_welcomeNewUser();
	}
	catch(const NumericReply& e)
	{
		addNumericMsg(e.code(), e.what());
	}
}

void	Message::_user(const std::string &arg)
{
	if (!_sender->authentificated)
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}

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
		addNumericMsg(e.code(), e.what());
	}
}

void	Message::_pong(const std::string &arg)
{
	if (!_sender->authentificated || !_sender->isWelcomed())
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}

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

void	Message::_join(const std::string &arg)
{
	if (!_sender->authentificated || !_sender->isWelcomed())
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}
	if (arg.empty())
	{
		addNumericMsg(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
		return ;
	}
	// if (!arg.compare("0"))
	// {
	// 	// client leaves all joined channel, equivalent to the PART command"
	// }
	
	std::vector<std::string>	tmp = _split(arg, " ");
	std::vector<std::string>	chan_name = _split(tmp[0], ","), key;
	if (tmp.size() > 1)
		key = _split(tmp[1], ",");
	while (key.size() < chan_name.size())
		key.push_back("");
	
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
				channel->addUser(_sender, NULL, key[i]);
			}
			std::map<User*, bool> chan_users = channel->getUsers();
			std::string			user_list;
			FOREACH(chan_users, it)
			{
				it->first->getMessage()->addMsg(_sender, "JOIN", channel->getName());
				user_list += it->first->getNickname() + " ";
			}
			if (!channel->getTopic().empty())
				addNumericMsg(RPL_TOPIC, channel->getName() + " " + channel->getTopic());
			addNumericMsg(RPL_NAMREPLY, channel->getName() + " :" + user_list);
			addNumericMsg(RPL_ENDOFNAMES);
		}
		catch(const NumericReply& e)
		{
			addNumericMsg(e.code(), e.what());
		}
		i++;
	}
}

void	Message::_topic(const std::string &arg)
{
	if (!_sender->authentificated || !_sender->isWelcomed())
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}

	std::stringstream	ss(arg);
	std::string			target, new_topic;

	try
	{
		if (!std::getline(ss, target, ' '))
			throw NumericReply(ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		Channel	&channel = _server->getChannel(target);
		if (!channel.isUserOnChannel(_sender->getNickname()))
			throw NumericReply(ERR_NOTONCHANNEL, channel.getName() + " :You're not on that channel"); // ERR optionnelle
		if (!std::getline(ss, new_topic))
		{
			if (channel.getTopic().empty())
				throw NumericReply(RPL_NOTOPIC, channel.getName() + ":No topic is set");
			else
				throw NumericReply(RPL_TOPIC, channel.getName() + channel.getTopic());
		}
		channel.setTopic(_sender, new_topic);
		std::map<User*, bool> chan_users = channel.getUsers();
		FOREACH(chan_users, it)
			it->first->getMessage()->addMsg(_sender, "TOPIC", channel.getName(), new_topic);
	}
	catch (const NumericReply &e)
	{
		addNumericMsg(e.code(), e.what());
	}
}

void	Message::_invite(const std::string &arg)
{
	if (!_sender->authentificated || !_sender->isWelcomed())
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}

	std::stringstream	ss(arg);
	std::string			target_nickname, target_chan;
	try
	{
		if (!std::getline(ss, target_nickname, ' ') || !std::getline(ss, target_chan, ' '))
			throw NumericReply(ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
		User	&target = _server->getUser(target_nickname);
		Channel &channel = _server->getChannel(target_chan);
		if (!channel.isUserOnChannel(_sender->getNickname()))
			throw NumericReply(ERR_NOTONCHANNEL, channel.getName() + " :You're not on that channel");
		User	&invited = _server->getUser(target.getNickname());
		channel.addUser(&invited, _sender);
		addNumericMsg(RPL_INVITING, target.getNickname() + " " + channel.getName());
		invited.getMessage()->addMsg(_sender, "INVITE", target.getNickname(), channel.getName());
		invited.getMessage()->addMsg(&invited, "JOIN", channel.getName());
	}
	catch (const NumericReply &e)
	{
		addNumericMsg(e.code(), e.what());
	}
}

void	Message::_kick(const std::string &arg)
{
	if (!_sender->authentificated || !_sender->isWelcomed())
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}

	std::stringstream	ss(arg);
	std::string			chan_name, comment, target_list;
	std::vector<std::string>	targets;
	try
	{
		if (!std::getline(ss, chan_name, ' ') || !std::getline(ss, target_list, ' '))
			throw NumericReply(ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
		targets = _split(target_list, ",");
		if (!std::getline(ss, comment) || (comment.size() <= 1 && !comment.compare(":")))
			comment = ":User kicked from chan\r\n";
		Channel	&channel = _server->getChannel(chan_name);
		if (!channel.isUserOnChannel(_sender->getNickname()))
			throw NumericReply(ERR_NOTONCHANNEL, chan_name + " :You're not on that channel");
		if (!channel.isChanop(_sender->getNickname()))
			throw NumericReply(ERR_CHANOPRIVSNEEDED,chan_name + " :You're not channel operator");
		std::map<User*, bool> chan_users = channel.getUsers();
		for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); it++)
		{
			if (!channel.isUserOnChannel(*it))
				addNumericMsg(ERR_USERNOTINCHANNEL, *it + " " + channel.getName() + " :They aren't on that channel");
			else
			{
				User	*rm_user = &_server->getUser(*it);
				rm_user->getMessage()->addMsg(_sender, "KICK", chan_name, rm_user->getNickname() + " " + comment);
				channel.removeUser(rm_user);
				FOREACH(chan_users, it2)
					it2->first->getMessage()->addMsg(_sender, "KICK", chan_name, rm_user->getNickname() + " " + comment);
			}
		}
	}
	catch(const NumericReply& e)
	{
		addNumericMsg(e.code(), e.what());
	}
}

void	Message::_mode(const std::string &arg)
{
	if (!_sender->authentificated || !_sender->isWelcomed())
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}

	std::stringstream	ss(arg);
	std::string			target, modestring;
	std::getline(ss, target, ' ');
	try
	{
		if (target[0] == '#')
		{
			Channel	&channel = _server->getChannel(target);
			if (!std::getline(ss, modestring, ' '))
				throw NumericReply(RPL_CHANNELMODEIS, channel.getName() + " " + channel.getActiveModes() + " " + channel.getModesDiffArg());
			else
			{
				if (!channel.isChanop(_sender->getNickname()))
					throw NumericReply(ERR_CHANOPRIVSNEEDED, channel.getName() + " :You're not a channel operator");
				if (channel.setModes(_sender, modestring, ss))
				{
					std::map<User*, bool> chan_users = channel.getUsers();
					FOREACH(chan_users, it)
						it->first->getMessage()->addMsg(_sender, "MODE", channel.getName(), ":" + channel.getModesDiff() + " " + channel.getModesDiffArg());
				}
			}
		}
		else
		{
			if (!_server->isUser(target))
				throw NumericReply(ERR_NOSUCHNICK, target + " :No such nick/channel");
			if (!isEquals(target, _sender->getNickname()))
				throw NumericReply(ERR_USERSDONTMATCH, ":Cant change/view mode for other users");
			if (!std::getline(ss, modestring, ' '))
				throw NumericReply(RPL_UMODEIS, _sender->getActiveModes());
			else
				if (_sender->setModes(modestring))
					addMsg(_sender, "MODE", _sender->getNickname(), _sender->getModesDiff());
		}
	}
	catch (const NumericReply &e)
	{
		addNumericMsg(e.code(), e.what());
	}
}

void	Message::_privmsg(const std::string &arg)
{
	if (!_sender->authentificated || !_sender->isWelcomed())
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}

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
			std::map<User*,bool> chan_users = _server->getChannel(target_name).getUsers();
			FOREACH(chan_users, it)
			{
				if (it->first != _sender)
					it->first->getMessage()->addMsg(_sender, "PRIVMSG", target_name, text_to_send);
			}
		}
		else
		{
			User	&target = _server->getUser(target_name);
			target.getMessage()->addMsg(_sender, "PRIVMSG", target.getNickname(), text_to_send);
		}
	}
	catch(const NumericReply& e)
	{
		addNumericMsg(e.code(), e.what());
	}
}

void	Message::_whois(const std::string &arg)
{
	if (!_sender->authentificated || !_sender->isWelcomed())
	{
		addNumericMsg(ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}

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
