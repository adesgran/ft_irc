/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:03:38 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/20 11:54:50 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(void)
{
	Log log;
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

	this->_pfds_init();

	fcntl(this->_serverfd, F_SETFL, O_NONBLOCK);

	_log.debug("Hello !");
	_log.info("Server is ready");
	_log.warning("Take care !");
	_log.error("Oh no ! It's too late !");
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

Log	&Server::getLog( void )
{
	return (this->_log);
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

void	Server::_remove_user( int fd )
{
	for ( 
			std::vector<Channel>::iterator it = this->_channels.begin(); 
			it != this->_channels.end(); 
			it++ )
		it->removeUser(fd);
	for ( 
			std::vector<User>::iterator it = this->_users.begin(); 
			it != this->_users.end(); 
			it++ )
	{
		if ( it->getSockfd() == fd )
		{
			this->_users.erase(it);
			return ;
		}
	}
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

void	Server::_pfds_init( void )
{
	this->_nfds = 1;
	this->_pfds = new struct pollfd [this->_nfds];
	this->_pfds[0].fd = this->_serverfd;
	this->_pfds[0].events = POLLIN;
}

void	Server::_pfds_add( int fd )
{
	struct pollfd *new_pfds = new struct pollfd [this->_nfds + 1];
	for ( nfds_t n = 0; n < this->_nfds; n++ )
		new_pfds[n] = this->_pfds[n];
	new_pfds[this->_nfds].fd = fd;
	new_pfds[this->_nfds].events = POLLIN;
	delete this->_pfds;
	this->_pfds = new_pfds;
	this->_nfds++;
}

void	Server::_pfds_remove( int fd )
{
	struct pollfd *new_pfds = new struct pollfd [this->_nfds - 1];
	nfds_t i = 0, n = 0;
	while ( n < this->_nfds )
	{
		if ( this->_pfds[n].fd != fd )
		{
			new_pfds[i] = this->_pfds[n];
			i++;
			n++;
		}
		else
			n++;
	}
	delete this->_pfds;
	this->_pfds = new_pfds;
	this->_nfds--;
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
			return ;
		std::cout << "Error on connection acceptation : " << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}
	std::cout << "New Connection Set   fd : " << new_sock << std::endl;
	this->_pfds_add( new_sock );
	this->_users.push_back(User(new_sock));
}

void	Server::_listenMessage( int fd )
{
	ssize_t	len;
	this->_buffer[0] = '\0';
	len = recv(fd, this->_buffer, BUFFER_SIZE, MSG_DONTWAIT);
	this->_buffer[len] = '\0';
	if ( len )
	{
		std::cout << "fd : " << fd << " >> " << this->_buffer;
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
		int ready = poll(this->_pfds, this->_nfds, 1000);
		if ( ready == -1 )
			throw Server::PollException();
		if ( ready )
		{
			if ( this->_pfds[0].revents & POLLIN )
			{
				this->_listenConnect();
			}
			for ( nfds_t n = 1; n < this->_nfds; n++ )
			{
				if ( this->_pfds[n].revents & POLLERR )
				{
					std::cout << "POLLERR" << std::endl;
					throw Server::PollException();
				}
				else if ( this->_pfds[n].revents & POLLIN )
					this->_listenMessage(this->_pfds[n].fd);
				else if ( this->_pfds[n].revents & POLLHUP )
				{
					std::cout << "Close fd : " << this->_pfds[n].fd << std::endl;
					this->_remove_user(this->_pfds[n].fd);
					close(this->_pfds[n].fd);
					this->_pfds_remove(this->_pfds[n].fd);
					n--;
				}

			}
		}
	}
}

