/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:59:54 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/04 17:11:02 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

Channel::Channel(void)
{
	this->_name = "";
	_modes['i'] = false;
	_modes['t'] = false;
	_modes['k'] = false;
	_modes['o'] = false;
	_modes['l'] = false;
}

Channel::Channel( const std::string name )
{
	this->_name = name;
	_modes['i'] = false;
	_modes['t'] = false;
	_modes['k'] = false;
	_modes['o'] = false;
	_modes['l'] = false;
}


Channel::Channel(const Channel &channel)
{
	*this = channel;
}

Channel::~Channel(void)
{
};

Channel &Channel::operator=(const Channel &channel)
{
	if ( this == &channel )
		return (*this);
	this->_name = channel.getName();
	this->_users = channel._users;
	return (*this);
}


const std::vector<User *>	Channel::getUsers( void ) const
{
	return (this->_users);
}

void	Channel::addUser( User *user )
{
	this->_users.push_back(user);
}

void	Channel::removeUser( User *user )
{
	for ( std::vector<User *>::iterator it = this->_users.begin();
			it != this->_users.end();
			it++)
	{
		if ( *it == user )
		{
			this->_users.erase(it);
			return ;
		}
	}
}

void	Channel::removeUser( int fd )
{
	for ( std::vector<User *>::iterator it = this->_users.begin();
			it != this->_users.end();
			it++)
	{
		if ( (*it)->getSockfd() == fd )
		{
			this->_users.erase(it);
			return ;
		}
	}
}

bool	Channel::isUserOnChannel(const std::string &nickname)
{
	for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (!nickname.compare((*it)->getNickname()))
			return (true);
	}
	return (false);
}

void	Channel::setName( const std::string name )
{
	this->_name = name;
}

std::string	Channel::getName( void ) const
{
	return (this->_name);
}

bool	Channel::setModes(std::string new_modes, std::string mode_arg)
{
	char	op = 0;
	bool	err = false;
	std::stringstream	ss(mode_arg);

	for (std::string::iterator it = new_modes.begin(); it != new_modes.end(); it++)
	{
		if (*it == '+' || *it == '-')
		{
			op = *it;
		}
		else if (op == '+' && _modes.find(*it) != _modes.end())
		{
			_modes[*it] = true;
		}
		else if (op == '-' && _modes.find(*it) != _modes.end())
		{
			_modes[*it] = false;
		}
		else if (*it != 'o')
		{
			err = true;
		}
	}
	(void)mode_arg;
	return (err);
}

std::string	Channel::getActiveModes()
{
	std::string ret;
	if (_modes['i'])
		ret += 'i';
	if (_modes['t'])
		ret += 't';
	if (_modes['k'])
		ret += 'k';
	if (_modes['o'])
		ret += 'o';
	if (_modes['l'])
		ret += 'l';
	return (ret);
}

void	Channel::setKey(User *sender, const std::string key)
{
	// if sender != operator
	// error
	if (!_modes['k'])
		return ;
	if (key.find(' ') == std::string::npos)
		_key = key;
	else
		throw std::exception(); // ERR_INVALIDMODEPARAM
	(void)sender;
}

std::string	Channel::getKey() const
{
	return (_key);
}

void	Channel::setTopic(User *sender, std::string new_topic)
{
	if (_modes['t']) // && sender == operator
		_topic = new_topic;
	(void)sender;
}

std::string	Channel::getTopic() const
{
	return (_topic);
}

void	Channel::setClientLimit(User *sender, size_t new_lim)
{
	// if sender != operator
	// error
	if (!_modes['l'])
		return ;
	_client_limit = new_lim;
	(void)sender;
}

size_t	Channel::getClientLimit() const
{
	return (_client_limit);
}


