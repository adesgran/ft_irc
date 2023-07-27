/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/01 13:44:20 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/27 15:29:39 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <User.hpp>

User::User(void)
{
	this->_sockfd = 1;
	this->_isop = false;
	this->_welcomed = false;
	this->_message = new Message(this);
}

User::User(int sockfd)
{
	this->_sockfd = sockfd;
	this->_isop = false;
	this->_welcomed = false;
	this->_message = new Message(this);
}

User::User(const User &user)
{
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
	this->_sockfd = user.getSockfd();
	this->_isop = user.getIsop();
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

std::string	User::getMode( void ) const
{
	return (this->_mode);
}

void		User::setMode( const std::string mode )
{
	this->_mode = mode;
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
