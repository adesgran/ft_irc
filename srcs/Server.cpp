/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:03:38 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/13 16:16:24 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(void)
{
	this->_opt = 1;
	this->_addrlen = sizeof(this->_sockaddr);

	if ( ( this->_serverfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
	{
		std::cout << "Error on socket creation : " << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}

	if ( setsockopt(
				this->_serverfd, 
				SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
				&this->_opt, 
				sizeof(this->_opt) ) )
	{
		std::cout << "Error on socket options: " << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}

	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
	this->_sockaddr.sin_port = htons(PORT);

	if ( bind(
				this->_serverfd, 
				(struct sockaddr*)&this->_sockaddr, 
				sizeof(this->_sockaddr) ) < 0 )
	{
		std::cout << "Error on binding socket: " << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}

	if ( listen(this->_serverfd, 10) < 0)
	{
		std::cout << "Error on marking socket as passive socket: " << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}

	fcntl(this->_serverfd, F_SETFL, O_NONBLOCK);

	std::cout << "Server ON" << std::endl;
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
	return (*this);
}

const std::vector<User>	&Server::getUsers( void ) const
{
	return (this->_users);
}

const std::vector<Channel>	&Server::getChannels( void ) const
{
	return (this->_channels);
}

User	&Server::getUser( const int sockfd )
{
	for ( 
			std::vector<User>::iterator it = this->_users.begin(); 
			it != this->_users.end(); 
			it++ )
	{
		if ( it->getSockfd() == sockfd )
			return (*it);
	}
	throw (Server::UserDoesNotExistException());
}

User	&Server::getUser( const std::string name )
{
	for ( 
			std::vector<User>::iterator it = this->_users.begin(); 
			it != this->_users.end(); 
			it++ )
	{
		if ( it->getNickname() == name )
			return (*it);
	}
	throw (Server::UserDoesNotExistException());
}

void	Server::addUser( const User &user )
{
	this->_users.push_back( User(user) );
}

Channel	&Server::getChannel( const std::string name )
{
	for (
			std::vector<Channel>::iterator it = this->_channels.begin(); 
			it != this->_channels.end(); 
			it++ )
	{
		if ( it->getName() == name )
			return (*it);
	}
	Channel	res(name);
	this->_channels.push_back(res);
	return (this->_channels.back());
}


void	Server::_listenConnect( void )
{
	int	new_sock;
	if ( ( new_sock = accept(
					this->_serverfd, 
					(struct sockaddr*)&this->_sockaddr, 
					(socklen_t*)&this->_addrlen) ) < 1 )
	{
		if ( errno == EAGAIN || errno == EWOULDBLOCK )
			return;
		std::cout << "Error on connection acceptation : " << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}
	std::cout << "New Connection Set" << std::endl;
	this->_fdvector.push_back(new_sock);
	this->_users.push_back(User(new_sock));
}

void	Server::_listenMessage( void )
{
	for ( 
			std::vector<int>::iterator it = this->_fdvector.begin(); 
			it != this->_fdvector.end(); 
			it++ )
	{
		int	len;
		this->_buffer[0] = '\0';
		len = recv(*it, this->_buffer, BUFFER_SIZE, MSG_DONTWAIT);
		this->_buffer[len] = '\0';
		if ( len )
		{
			std::cout << this->_buffer;
		}
	}
}

void	Server::stop( void )
{
	throw Server::SigIntException();
}

void	sigintHandle( int sig )
{
	std::cout << std::endl;
	(void)sig;
	Server::stop();
}

void	Server::run( void )
{
	signal(SIGINT, sigintHandle);
	while ( 1 )
	{
		this->_listenConnect();
		this->_listenMessage();
	}
}

