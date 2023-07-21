/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/01 13:43:30 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/21 18:35:43 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include <Server.hpp>
# include <Channel.hpp>
# include <Message.hpp>

class Server;
class Channel;
class Message;

class User {
	public:
		User( void );
		User( int sockfd );
		User( const User &user );
		~User( void );
		User &operator=( const User &user );

		int		getSockfd( void ) const;
		void	setSockfd( int sockfd );
		bool	getIsop( void ) const;
		void	setIsop( bool isop );

		std::string	getUsername( void ) const;
		void		setUsername( const std::string username );
		std::string	getNickname( void ) const;
		void		setNickname( const std::string nickname );
		std::string	getRealname( void ) const;
		void		setRealname( const std::string realname );
		std::string	getHostname( void ) const;
		void		setHostname( const std::string hostname );
		int			getMode( void ) const;
		void		setMode( const int mode );
		Message*	getMessage( void ) const;
		void		setMessage(Message *msg);
		bool		isWelcomed( void ) const;
		void		welcome( void );

	private:
		int			_sockfd;
		bool		_isop;
		std::string	_username;
		std::string	_nickname;
		std::string	_realname;
		std::string	_hostname;
		int			_mode;
		Message*	_message;
		bool		_welcomed;

};

#endif
