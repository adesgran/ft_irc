/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:03:38 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/13 00:00:10 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(void)
{
	this->_log = new Log();
	this->_opt = 1;
	this->_addrlen = sizeof(this->_sockaddr);

	if ( ( this->_serverfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
	{
		this->_log->error("Error on socket creation");
		this->_log->error(strerror(errno));
		throw std::runtime_error(strerror(errno));
	}

	if ( setsockopt(
				this->_serverfd, 
				SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
				&this->_opt, 
				sizeof(this->_opt) ) )
	{
		this->_log->error("Error on socket options");
		this->_log->error(strerror(errno));
		throw std::runtime_error(strerror(errno));
	}

	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
	this->_sockaddr.sin_port = htons(6667);

	if ( bind(
				this->_serverfd, 
				(struct sockaddr*)&this->_sockaddr, 
				sizeof(this->_sockaddr) ) < 0 )
	{
		this->_log->error("Error on binding socket");
		this->_log->error(strerror(errno));
		throw std::runtime_error(strerror(errno));
	}

	if ( listen(this->_serverfd, 10) < 0)
	{
		this->_log->error("Error on marking socket as passive socket");
		this->_log->error(strerror(errno));
		throw std::runtime_error(strerror(errno));
	}

	this->_pfds_init();

	fcntl(this->_serverfd, F_SETFL, O_NONBLOCK);

	_log->info("Server is ready");
}

Server::Server( int port )
{
	this->_log = new Log();
	this->_opt = 1;
	this->_addrlen = sizeof(this->_sockaddr);

	if ( ( this->_serverfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
	{
		this->_log->error("Error on socket creation");
		this->_log->error(strerror(errno));
		throw std::runtime_error(strerror(errno));
	}

	if ( setsockopt(
				this->_serverfd, 
				SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
				&this->_opt, 
				sizeof(this->_opt) ) )
	{
		this->_log->error("Error on socket options");
		this->_log->error(strerror(errno));
		throw std::runtime_error(strerror(errno));
	}

	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
	this->_sockaddr.sin_port = htons(port);

	if ( bind(
				this->_serverfd, 
				(struct sockaddr*)&this->_sockaddr, 
				sizeof(this->_sockaddr) ) < 0 )
	{
		this->_log->error("Error on binding socket");
		this->_log->error(strerror(errno));
		throw std::runtime_error(strerror(errno));
	}

	if ( listen(this->_serverfd, 10) < 0)
	{
		this->_log->error("Error on marking socket as passive socket");
		this->_log->error(strerror(errno));
		throw std::runtime_error(strerror(errno));
	}

	this->_pfds_init();

	fcntl(this->_serverfd, F_SETFL, O_NONBLOCK);

	_log->info("Server is ready");
}

Server::Server(const Server &server)
{
	*this = server;
}

Server::~Server(void)
{
	delete[] this->_pfds;
	for ( std::vector<User *>::iterator it = this->_users.begin(); it != this->_users.end(); it++ )
	{
		delete *it;
	}
	for ( std::vector<Channel *>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++ )
	{
		delete *it;
	}
	delete this->_log;
};

Server &Server::operator=(const Server &server)
{
	if ( this == &server )
		return ( *this );
	return (*this);
}

void		Server::setPassword(const std::string &password)
{
	_password = password;
}

const std::string	&Server::getPassword() const
{
	return (_password);
}

Log	*Server::getLog( void ) const
{
	return (this->_log);
}

const std::vector<User *>	&Server::getUsers( void ) const
{
	return (this->_users);
}

const std::vector<Channel *>	&Server::getChannels( void ) const
{
	return (this->_channels);
}

User	&Server::getUser( const int sockfd ) const
{
	for ( 
			std::vector<User *>::const_iterator it = this->_users.begin(); 
			it != this->_users.end(); 
			it++ )
	{
		if ( (*it)->getSockfd() == sockfd )
			return (**it);
	}
	throw (Server::UserDoesNotExistException());
}

User	&Server::getUser( const std::string name ) const
{
	for ( 
			std::vector<User *>::const_iterator it = this->_users.begin(); 
			it != this->_users.end(); 
			it++ )
	{
		if ( (*it)->getNickname() == name )
			return (**it);
	}
	throw (Server::UserDoesNotExistException());
}

bool	Server::isUser(const std::string nickname) const
{
	for ( 
			std::vector<User *>::const_iterator it = this->_users.begin(); 
			it != this->_users.end(); 
			it++ )
	{
		if ( (*it)->getNickname() == nickname )
			return (true);
	}
	return (false);
}

void	Server::_addUser( int fd )
{
	User *user = new User(fd);
	if (_password.empty())
		user->authentificated = true;
	this->_users.push_back( user );
}

void	Server::_remove_user( int fd )
{
	for ( 
			std::vector<Channel *>::iterator it = this->_channels.begin(); 
			it != this->_channels.end(); 
			it++ )
		(*it)->removeUser(fd);
	for ( 
			std::vector<User *>::iterator it = this->_users.begin(); 
			it != this->_users.end(); 
			it++ )
	{
		if ( (*it)->getSockfd() == fd )
		{
			delete *it;
			this->_users.erase(it);
			return ( this->_remove_user( fd ) );
		}
	}
}

void	Server::addChannel(Channel *new_chan)
{
	this->_channels.push_back(new_chan);
}

Channel	&Server::getChannel( const std::string name ) const
{
	for (
			std::vector<Channel *>::const_iterator it = this->_channels.begin(); 
			it != this->_channels.end(); 
			it++ )
	{
		if ( (*it)->getName() == name )
			return (**it);
	}
	throw Message::NumericReply(ERR_NOSUCHCHANNEL, name + " :No such channel");
}

bool	Server::isChannel(const std::string name) const
{
	for (
			std::vector<Channel *>::const_iterator it = this->_channels.begin(); 
			it != this->_channels.end(); 
			it++ )
	{
		if ( (*it)->getName() == name )
			return (true);
	}
	return (false);
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
	new_pfds[this->_nfds].events = POLLIN | POLLOUT;
	delete[] this->_pfds;
	this->_pfds = new_pfds;
	this->_nfds += 1;
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
	delete[] this->_pfds;
	this->_pfds = new_pfds;
	this->_nfds -= 1;
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
		this->_log->error("Error on connection acceptation ");
		this->_log->error(strerror(errno));
		throw std::runtime_error(strerror(errno));
	}
	this->_log->info("New connection set");
	this->_pfds_add( new_sock );
	this->_addUser( new_sock );
}

int	Server::_listenMessage( int fd )
{
	ssize_t	len;
	char	buffer[BUFFER_SIZE];
	buffer[0] = '\0';
	len = recv(fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	buffer[len] = '\0';
	if ( len )
	{
		try
		{
			std::string input(buffer);
			User &client = this->getUser(fd);

			this->_log->client(input, client);
			Message *msg = this->getUser( fd ).getMessage();
			msg->setInputMsg( input, this );
			return (0);
		}
		catch (std::exception const & e)
		{
			this->_log->error(e.what());
			return (1);
		}

	}
	else
	{
		this->_log->debug("Empty Message received");
		return (1);
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
			nfds_t len = this->_nfds;
			if ( this->_pfds[0].revents & POLLIN )
			{
				this->_listenConnect();
			}
			for ( nfds_t n = 1; n < len; n++ )
			{
				if ( this->_pfds[n].revents & POLLERR || this->_pfds[n].revents & POLLHUP )
				{
					if ( this->_pfds[n].revents & POLLERR )
					{
						this->_log->debug("POLLERR");
					}
					{
						this->_log->info("Connection closed");
						this->_remove_user(this->_pfds[n].fd);
						close(this->_pfds[n].fd);
						this->_pfds_remove(this->_pfds[n].fd);
						n = len;
					}
				}
				else
				{
					if ( this->_pfds[n].revents & POLLIN )
					{
						this->_log->debug("Listen message");
						if (this->_listenMessage(this->_pfds[n].fd))
						{
							this->_log->info("Connection closed");
							this->_remove_user(this->_pfds[n].fd);
							close(this->_pfds[n].fd);
							this->_pfds_remove(this->_pfds[n].fd);
							n = len;
						}
					}
					if ( this->_pfds[n].revents & POLLOUT )
					{
						Message *msg = this->getUser( this->_pfds[n].fd ).getMessage();

						std::string output = msg->getOutputMsg();
						if ( !output.empty() )
						{
							this->_log->server(output, this->getUser( this->_pfds[n].fd ));
							send( this->_pfds[n].fd, output.c_str(), output.size(), 0 );
						}
					}
				}
			}
		}
	}
}

