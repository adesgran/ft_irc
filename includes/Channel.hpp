/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:53:07 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/03 15:03:09 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <User.hpp>
# include <Server.hpp>

class User;
class Server;

class Channel {
	public:
		Channel( void );
		Channel( const std::string name );
		Channel( const Channel &channel );
		~Channel( void );
		Channel &operator=( const Channel &channel );

		const std::vector<User *>	getUsers( void ) const;
		void	addUser( User *user );
		void	removeUser( User *user );
		void	removeUser( int fd );

		void		setName( const std::string name );
		std::string	getName( void ) const;
		void		setKey(const std::string key);
		std::string	getKey() const;

		std::string			topic;
		std::string			status;
		std::string			modes;

	private:
		std::vector<User *>	_users;
		std::string			_name;
		std::string			_key;
};

#endif
