/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:59:54 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/02 14:04:16 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

Channel::Channel(void)
{
	this->_name = "";
}

Channel::Channel( const std::string name )
{
	this->_name = name;
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
	return (*this);
}


const std::vector<User>	Channel::getUsers( void ) const
{
	return (this->_users);
}

void	Channel::addUser( User &user )
{
	this->_users.push_back(user);
}

void	Channel::removeUser( User &user )
{
	this->removeUser(user.getSockfd());
}

void	Channel::removeUser( int sockfd )
{
	for (std::vector<User>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if (it->getSockfd() == sockfd)
		{
			this->_users.erase(it);
			return ;
		}
	}
}

void	Channel::setName( const std::string name )
{
	this->_name = name;
}

std::string	Channel::getName( void ) const
{
	return (this->_name);
}
