/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/01 13:44:20 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/07 14:01:03 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <User.hpp>

User::User(void)
{
	this->_sockfd = 1;
	this->_isop = false;
	this->_welcomed = false;
	this->_message = new Message(this);
	_modes['i'] = false;
	_modes['w'] = false;
	_modes['o'] = false;
	_modes['O'] = false;
}

User::User(int sockfd)
{
	this->_sockfd = sockfd;
	this->_isop = false;
	this->_welcomed = false;
	this->_message = new Message(this);
	_modes['i'] = false;
	_modes['w'] = false;
	_modes['o'] = false;
	_modes['O'] = false;
}

User::User(const User &user)
{
	this->_message = NULL;
	*this = user;
}

User::~User(void)
{
	delete this->_message;
};

User &User::operator=(const User &user)
{
	if (this == &user)
		return (*this);
	if (this->_message != NULL)
		delete this->_message;
	this->_message = new Message(*(user._message));
	this->_sockfd = user.getSockfd();
	this->_isop = user.getIsop();
	this->_welcomed = user._welcomed;
	return (*this);
}


void	User::setSockfd( int sockfd )
{
	this->_sockfd = sockfd;
}

int	User::getSockfd( void ) const
{
	return (this->_sockfd);
}

void	User::setIsop( bool isop )
{
	this->_isop = isop;
}

bool	User::getIsop( void ) const
{
	return (this->_isop);
}

std::string	User::getUsername( void ) const
{
	return (this->_username);
}

void		User::setUsername( const std::string username )
{
	this->_username = username;
}

std::string	User::getNickname( void ) const
{
	return (this->_nickname);
}

void		User::setNickname( const std::string nickname )
{
	this->_nickname = nickname;
}

std::string	User::getRealname( void ) const
{
	return (this->_realname);
}

void		User::setRealname( const std::string realname )
{
	this->_realname = realname;
}

std::string	User::getHostname( void ) const
{
	return (this->_hostname);
}

void		User::setHostname( const std::string hostname )
{
	this->_hostname = hostname;
}

std::string	User::getActiveModes( void ) const
{
	std::string ret;
	if (_modes.at('i'))
		ret += 'i';
	if (_modes.at('w'))
		ret += 'w';
	if (_modes.at('o'))
		ret += 'o';
	if (_modes.at('O'))
		ret += 'O';
	return (ret);
}

bool	User::setModes(const std::string new_modes)
{
	char	op = 0;
	bool	err = false;

	for (std::string::const_iterator it = new_modes.begin(); it != new_modes.end(); it++)
	{
		if (*it == '+' || *it == '-')
		{
			op = *it;
		}
		else if (op == '+' && _modes.find(*it) != _modes.end() && *it != 'o' && *it != 'O')
		{
			_modes[*it] = true;
		}
		else if (op == '-' && _modes.find(*it) != _modes.end())
		{
			_modes[*it] = false;
		}
		else if (*it != 'o' && *it != 'O')
		{
			_message->appendOutputMsg(ERR_UMODEUNKNOWNFLAG, _nickname + " :Unknown MODE flag");
		}
	}
	return (err);
}

Message	*User::getMessage( void ) const
{
	return (_message);
}

bool	User::isWelcomed( void ) const
{
	return (_welcomed);
}

void		User::welcome( void )
{
	_welcomed = true;
}
