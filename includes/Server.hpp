/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 11:59:06 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/15 13:26:42 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <User.hpp>
# include <Channel.hpp>
# include <Log.hpp>
# include <stdlib.h>
# include <stdio.h>
# include <cstring>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <cerrno>
# include <stdexcept>
# include <signal.h>
# include <poll.h>

#define BUFFER_SIZE 2000

class User;
class Channel;
class Log;
bool isEquals(const std::string& a, const std::string& b);

class Server {
	public:
		Server( void );
		Server( int port );
		Server( const Server &server );
		~Server( void );
		Server &operator=( const Server &server );

		void				setPassword(const std::string &password);
		const std::string	&getPassword() const;
		
		const std::vector<User *>	&getUsers( void ) const;
		const std::vector<Channel *>	&getChannels( void ) const;

		User	&getUser( const int sockfd ) const;
		User	&getUser( const std::string name ) const;
		bool	isUser(const std::string nickname) const;
		Log		*getLog( void ) const;

		void	addChannel(Channel *new_chan);
		Channel	&getChannel( const std::string name ) const;
		bool	isChannel(const std::string name) const;
		void	removeChannel( Channel &chan );

		static void	stop( void );

		void	run( void );


	private:
		std::string				_password;
		std::vector<User *>		_users;
		std::vector<Channel *>	_channels;
		int						_serverfd;
		std::vector<int>		_fdvector;
		struct sockaddr_in		_sockaddr;
		socklen_t				_addrlen;
		int						_opt;
		struct pollfd			*_pfds;
		nfds_t					_nfds;
		Log						*_log;

		void	_addUser( int fd );
		void	_remove_user( int fd );
		void	_listenConnect( void );
		int		_listenMessage( int fd );
		void	_pfds_init( void );
		void	_pfds_add( int fd );
		void	_pfds_remove( int fd );
		void	_disconnect( struct pollfd &pfd );


		class	UserDoesNotExistException : public std::exception
		{
			public :
				virtual const char *what() const throw()
				{
					return ("User does not exist");
				}
		};

		class	SigIntException : public std::exception
		{
			public :
				virtual const char *what() const throw()
				{
					return ("SigInt stop the program");
				}
		};

		class	PollException : public std::exception
		{
			public :
				virtual const char *what() const throw()
				{
					return ("Poll return -1");
				}
		};
};

#endif
