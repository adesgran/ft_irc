/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 16:50:33 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/01 19:52:44 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <stdexcept>
# include <vector>
# include <stdlib.h>
# include <cstring>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <cerrno>
# include <signal.h>
# include <poll.h>
# include <utils.hpp>

# define BUFFER_SIZE 2001

class Bot {
	public:
		Bot( void );
		Bot( const Bot &bot );
		~Bot( void );
		Bot &operator=( const Bot &bot );

		int		conf( std::string filename );
		void	run( void );


	private:
		int					_fd;
		struct sockaddr_in	_sockaddr;
		socklen_t			_addrlen;
		int					_opt;
		struct pollfd		_pfds;
		std::stringstream	_output;

		void				_readline( std::string line );
		void				_listenMessage( void );
		void				_sendMessage( void );


		class	InvalidFileException : public std::exception
		{
			public :
				virtual const char *what() const throw()
				{
					return ("Invalid File");
				}
		};

};

#endif
