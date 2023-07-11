/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:03:38 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/02 13:54:59 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(void)
{
}

Server::Server(const Server &server)
{
	*this = server;
}

Server::~Server(void)
{
};

Server &Server::operator=(const Server &server)
{
	if ( this == &server )
		return ( *this );
	this->_users = server.getUsers();
	this->_channels = server.getChannels();
	return (*this);
}

const std::map<int, User>	&Server::getUsers( void ) const
{
	return (this->_users);
}

const std::vector<Channel>	&Server::getChannels( void ) const
{
	return (this->_channels);
}

User	&Server::getUser( const int sockfd )
{
	User	&res = this->_users[sockfd];
	res.setSockfd(sockfd);
	return (res);
}

User	&Server::getUser( const std::string name )
{
	(void)name;
	return (this->_users[0]);
}

void	Server::addUser( const User &user )
{
	this->_users[user.getSockfd()] = user;
}

Channel	&Server::getChannel( const std::string name )
{
	for (std::vector<Channel>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if ( it->getName() == name )
			return (*it);
	}
	Channel	res(name);
	this->_channels.push_back(res);
	return (this->_channels.back());
}
