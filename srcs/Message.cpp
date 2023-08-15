/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/15 15:45:41 by adesgran         ###   ########.fr       */
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
	_cmdMap["PART"]		= &Message::_part;
	_cmdMap["TOPIC"]	= &Message::_topic;
	_cmdMap["INVITE"]	= &Message::_invite;
	_cmdMap["KICK"]		= &Message::_kick;
	_cmdMap["MODE"]		= &Message::_mode;
	_cmdMap["PRIVMSG"]	= &Message::_privmsg;
	_cmdMap["QUIT"]		= &Message::_quit;
}

Message::Message(User *sender): _sender(sender)
{
	_cmdMap["PASS"]		= &Message::_pass;
	_cmdMap["NICK"]		= &Message::_nick;
	_cmdMap["USER"]		= &Message::_user;
	_cmdMap["PING"]		= &Message::_pong;
	_cmdMap["JOIN"]		= &Message::_join;
	_cmdMap["PART"]		= &Message::_part;
	_cmdMap["TOPIC"]	= &Message::_topic;
	_cmdMap["INVITE"]	= &Message::_invite;
	_cmdMap["KICK"]		= &Message::_kick;
	_cmdMap["MODE"]		= &Message::_mode;
	_cmdMap["PRIVMSG"]	= &Message::_privmsg;
	_cmdMap["QUIT"]		= &Message::_quit;
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

void	Message::addReply(const User *source, const std::string cmd, const std::string target, const std::string arg)
{
	_output << ':' << SOURCE(source);
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
			addReply(_sender, ERR_UNKNOWNCOMMAND, _sender->getNickname(), cmd_name + " :Unknown command");
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
		<< " :Welcome to the ft_IRC Network " << SOURCE(_sender) << CRLF;
	}
}

void	Message::_pass(const std::string &arg)
{
	try
	{
		if (arg.empty())
			throw NumericReply(ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
		if (_sender->isAuthenticated())
			throw NumericReply(ERR_ALREADYREGISTERED, ":You may not reregister");
	}
	catch(const NumericReply& e)
	{
		if (_sender->getNickname().empty())
			addReply(_sender, e.code(), "*", e.what());
		else
			addReply(_sender, e.code(), _sender->getNickname(), e.what());
	}	
	if (arg.compare(_server->getPassword()))
	{
		addReply(_sender, ERR_PASSWDMISMATCH, _sender->getNickname(), ":Password incorrect");
		throw Disconnect();
	}
	_sender->setAuthenticated(true);
}

void	Message::_nick(const std::string &arg)
{
	if (!_sender->isAuthenticated())
	{
		addReply(_sender, ERR_NOTREGISTERED, _sender->getNickname(), ":You have not registered");
		throw Disconnect();
	}

	try
	{
		if (arg.empty())
			throw NumericReply(ERR_NONICKNAMEGIVEN, ":No nickname given");
		if (arg.find_first_of(",#:@ \t\n\r\v\f") != std::string::npos)
			throw NumericReply(ERR_ERRONEUSNICKNAME, arg + " :Erroneus nickname");
		if (!isEquals(arg, _sender->getNickname()) && _server->isUser(arg))
			throw NumericReply(ERR_NICKNAMEINUSE, arg + " :Nickname is already in use");
		if (!_sender->getNickname().empty())
			addReply(_sender, "NICK", arg);
		_sender->setNickname(arg);
		_welcomeNewUser();
	}
	catch(const NumericReply& e)
	{
		if (_sender->getNickname().empty())
			addReply(_sender, e.code(), "*", e.what());
		else
			addReply(_sender, e.code(), _sender->getNickname(), e.what());
	}
}

void	Message::_user(const std::string &arg)
{
	if (!_sender->isAuthenticated())
	{
		addReply(_sender, ERR_NOTREGISTERED, _sender->getNickname(), ":You have not registered");
		throw Disconnect();
	}

	std::stringstream	ss(arg);
	std::string			username, mode, hostname, realName;
	try
	{
		if (_sender->isWelcomed())
			throw NumericReply(ERR_ALREADYREGISTERED, ":You may not reregister");
		if (!std::getline(ss, username, ' ') || !std::getline(ss, mode, ' ') \
				|| !std::getline(ss, hostname, ' ') || !std::getline(ss, realName))
			throw NumericReply(ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		_sender->setUsername(username);
		(void)mode;
		_sender->setHostname(hostname);
		if (realName[0] == ':')
			realName.erase(0, 1);
		_sender->setRealname(realName);
		
		_welcomeNewUser();
	}
	catch(const NumericReply& e)
	{
		if (_sender->getNickname().empty())
			addReply(_sender, e.code(), "*", e.what());
		else
			addReply(_sender, e.code(), _sender->getNickname(), e.what());
	}
}

void	Message::_pong(const std::string &arg)
{
	std::stringstream	ss(arg);
	std::string			ret, mode_list;
	std::getline(ss, ret, ' ');
	if (ret.empty())
	{
		addReply(_sender, ERR_NEEDMOREPARAMS, _sender->getNickname(), " PING :Not enough parameters");
		return ;
	}
	_output << "PONG " << ret << CRLF;
}

void	Message::_join(const std::string &arg)
{
	if (!_sender->isAuthenticated() || !_sender->isWelcomed())
	{
		addReply(_sender, ERR_NOTREGISTERED, _sender->getNickname(), ":You have not registered");
		return ;
	}
	if (arg.empty())
	{
		addReply(_sender, ERR_NEEDMOREPARAMS, _sender->getNickname(), "JOIN :Not enough parameters");
		return ;
	}
	if (!arg.compare("0"))
	{
		std::vector<Channel*>	list_channels = _server->getChannels();
		for (std::vector<Channel*>::iterator it = list_channels.begin(); it != list_channels.end(); it++)
		{
			if ((*it)->isMember(_sender->getNickname()))
				_part((*it)->getName());
		}
		return ;
	}
	
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
				channel->addMember(_sender, NULL, key[i]);
			}
			std::map<User*, bool> members = channel->getMembers();
			std::string			user_list;
			FOREACH(members, it)
			{
				it->first->getMessage()->addReply(_sender, "JOIN", channel->getName());
				if (channel->isChanop(it->first->getNickname()))
					user_list += "@";
				user_list += it->first->getNickname() + " ";
			}
			if (!channel->getTopic().empty())
				addReply(_sender, RPL_TOPIC, _sender->getNickname(), channel->getName() + " " + channel->getTopic());
			user_list.erase(user_list.end()-1, user_list.end());
			addReply(_sender, RPL_NAMREPLY, _sender->getNickname(), "= " + channel->getName() + " :" + user_list);
			addReply(_sender, RPL_ENDOFNAMES, _sender->getNickname(), channel->getName() + " :End of /NAMES list");
		}
		catch(const NumericReply& e)
		{
			addReply(_sender, e.code(), _sender->getNickname(), e.what());
		}
		i++;
	}
}

void	Message::_part(const std::string &arg)
{
	if (!_sender->isAuthenticated() || !_sender->isWelcomed())
	{
		addReply(_sender, ERR_NOTREGISTERED, _sender->getNickname(), ":You have not registered");
		return ;
	}
	
	std::stringstream	ss(arg);
	std::string			tmp, reason;
	if (!std::getline(ss, tmp, ' '))
	{
		addReply(_sender, ERR_NEEDMOREPARAMS, _sender->getNickname(), "PART :Not enough parameters");
		return ;
	}
	std::vector<std::string>	chan_names = _split(tmp, ",");
	if (!std::getline(ss, reason) || (reason.size() <= 1 && !reason.compare(":")))
		reason = ":Leaving\r\n";
	for (std::vector<std::string>::iterator it = chan_names.begin(); it != chan_names.end(); it++)
	{
		try
		{
			Channel	&channel = _server->getChannel(*it);
			if (!channel.isMember(_sender->getNickname()))
				throw NumericReply(ERR_NOTONCHANNEL, channel.getName() + " :You're not on that channel");
			std::map<User*,bool>	members = channel.getMembers();
			FOREACH(members, it)
				it->first->getMessage()->addReply(_sender, "PART", channel.getName(), reason);
			channel.removeMember(_sender);
			if (channel.getMembers().size() == 0)
				_server->removeChannel(channel);
		}
		catch(const NumericReply& e)
		{
			addReply(_sender, e.code(), _sender->getNickname(), e.what());
		}
	}
}

void	Message::_topic(const std::string &arg)
{
	if (!_sender->isAuthenticated() || !_sender->isWelcomed())
	{
		addReply(_sender, ERR_NOTREGISTERED, _sender->getNickname(), ":You have not registered");
		return ;
	}

	std::stringstream	ss(arg);
	std::string			target, new_topic;

	try
	{
		if (!std::getline(ss, target, ' '))
			throw NumericReply(ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		Channel	&channel = _server->getChannel(target);
		if (!channel.isMember(_sender->getNickname()))
			throw NumericReply(ERR_NOTONCHANNEL, channel.getName() + " :You're not on that channel"); // ERR optionnelle
		if (!std::getline(ss, new_topic))
		{
			if (channel.getTopic().empty())
				throw NumericReply(RPL_NOTOPIC, channel.getName() + ":No topic is set");
			else
				throw NumericReply(RPL_TOPIC, channel.getName() + channel.getTopic());
		}
		channel.setTopic(_sender, new_topic);
		std::map<User*, bool> members = channel.getMembers();
		FOREACH(members, it)
			it->first->getMessage()->addReply(_sender, "TOPIC", channel.getName(), new_topic);
	}
	catch (const NumericReply &e)
	{
		addReply(_sender, e.code(), _sender->getNickname(), e.what());
	}
}

void	Message::_invite(const std::string &arg)
{
	if (!_sender->isAuthenticated() || !_sender->isWelcomed())
	{
		addReply(_sender, ERR_NOTREGISTERED, _sender->getNickname(), ":You have not registered");
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
		if (!channel.isMember(_sender->getNickname()))
			throw NumericReply(ERR_NOTONCHANNEL, channel.getName() + " :You're not on that channel");
		User	&invited = _server->getUser(target.getNickname());
		channel.addMember(&invited, _sender);
		addReply(_sender, RPL_INVITING, _sender->getNickname(), target.getNickname() + " " + channel.getName());
		invited.getMessage()->addReply(_sender, "INVITE", target.getNickname(), channel.getName());
		invited.getMessage()->addReply(&invited, "JOIN", channel.getName());
	}
	catch (const NumericReply &e)
	{
		addReply(_sender, e.code(), _sender->getNickname(), e.what());
	}
}

void	Message::_kick(const std::string &arg)
{
	if (!_sender->isAuthenticated() || !_sender->isWelcomed())
	{
		addReply(_sender, ERR_NOTREGISTERED, _sender->getNickname(), ":You have not registered");
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
		if (!channel.isMember(_sender->getNickname()))
			throw NumericReply(ERR_NOTONCHANNEL, chan_name + " :You're not on that channel");
		if (!channel.isChanop(_sender->getNickname()))
			throw NumericReply(ERR_CHANOPRIVSNEEDED,chan_name + " :You're not channel operator");
		std::map<User*, bool> members = channel.getMembers();
		for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); it++)
		{
			if (!channel.isMember(*it))
				addReply(_sender, ERR_USERNOTINCHANNEL, _sender->getNickname(), *it + " " + channel.getName() + " :They aren't on that channel");
			else
			{
				User	*rm_user = &_server->getUser(*it);
				rm_user->getMessage()->addReply(_sender, "KICK", chan_name, rm_user->getNickname() + " " + comment);
				channel.removeMember(rm_user);
				FOREACH(members, it2)
					it2->first->getMessage()->addReply(_sender, "KICK", chan_name, rm_user->getNickname() + " " + comment);
			}
		}
	}
	catch(const NumericReply& e)
	{
		addReply(_sender, e.code(), _sender->getNickname(), e.what());
	}
}

void	Message::_mode(const std::string &arg)
{
	if (!_sender->isAuthenticated() || !_sender->isWelcomed())
	{
		addReply(_sender, ERR_NOTREGISTERED, _sender->getNickname(), ":You have not registered");
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
					std::map<User*, bool> members = channel.getMembers();
					FOREACH(members, it)
						it->first->getMessage()->addReply(_sender, "MODE", channel.getName(), ":" + channel.getModesDiff() + " " + channel.getModesDiffArg());
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
					addReply(_sender, "MODE", _sender->getNickname(), _sender->getModesDiff());
		}
	}
	catch (const NumericReply &e)
	{
		addReply(_sender, e.code(), _sender->getNickname(), e.what());
	}
}

void	Message::_privmsg(const std::string &arg)
{
	if (!_sender->isAuthenticated() || !_sender->isWelcomed())
	{
		addReply(_sender, ERR_NOTREGISTERED, _sender->getNickname(), ":You have not registered");
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
			std::map<User*,bool> members = _server->getChannel(target_name).getMembers();
			FOREACH(members, it)
			{
				if (it->first != _sender)
					it->first->getMessage()->addReply(_sender, "PRIVMSG", target_name, text_to_send);
			}
		}
		else
		{
			User	&target = _server->getUser(target_name);
			target.getMessage()->addReply(_sender, "PRIVMSG", target.getNickname(), text_to_send);
		}
	}
	catch(const NumericReply& e)
	{
		addReply(_sender, e.code(), _sender->getNickname(), e.what());
	}
}

void	Message::_quit(const std::string &arg)
{
	addReply(_sender, "QUIT", arg);
	throw Disconnect();
}
