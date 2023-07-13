/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 11:59:06 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/13 16:05:24 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <User.hpp>
# include <Channel.hpp>
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

#define PORT 6667
#define BUFFER_SIZE 2000

class User;
class Channel;

class Server {
	public:
		Server( void );
		Server( const Server &server );
		~Server( void );
		Server &operator=( const Server &server );

		
		const std::vector<User>	&getUsers( void ) const;
		const std::vector<Channel>	&getChannels( void ) const;

		User	&getUser( const int sockfd );
		User	&getUser( const std::string name );
		void	addUser( const User &user );

		Channel	&getChannel( const std::string name );

		static void	stop( void );

		void	run( void );


	private:
		std::vector<User>		_users;
		std::vector<Channel>	_channels;
		int						_serverfd;
		std::vector<int>		_fdvector;
		struct sockaddr_in		_sockaddr;
		socklen_t				_addrlen;
		int						_opt;
		char					_buffer[BUFFER_SIZE];

		void	_listenConnect( void );
		void	_listenMessage( void );


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
};

#endif
