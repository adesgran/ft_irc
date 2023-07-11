/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 11:59:06 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/02 13:49:25 by adesgran         ###   ########.fr       */
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

class User;
class Channel;

class Server {
	public:
		Server( void );
		Server( const Server &server );
		~Server( void );
		Server &operator=( const Server &server );

		
		const std::map<int, User>	&getUsers( void ) const;
		const std::vector<Channel>	&getChannels( void ) const;

		User	&getUser( const int sockfd );
		User	&getUser( const std::string name );
		void	addUser( const User &user );

		Channel	&getChannel( const std::string name );


	private:
		std::map<int, User>			_users;
		std::vector<Channel>	_channels;


};

#endif
