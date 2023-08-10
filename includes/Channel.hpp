/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:53:07 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/10 13:46:40 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <map>
# include <User.hpp>
# include <Server.hpp>

class User;
class Server;

class Channel {
	public:
		Channel( void );
		Channel( const std::string name, User *founder );
		Channel( const Channel &channel );
		~Channel( void );
		Channel &operator=( const Channel &channel );

		const std::vector<User *>	getUsers( void ) const;
		void	addUser( User *target, User *sender = NULL, const std::string &key = "");
		void	removeUser( const User *user );
		void	removeUser( int fd );
		bool	isUserOnChannel(const std::string &nickname) const;

		void		setName( const std::string &name );
		std::string	getName( void ) const;

		std::string	setModes(const User *sender, const std::string &new_modes, std::stringstream &ss);
		std::string	getActiveModes() const;
		bool		isActiveMode(char c) const;
		void		setTopic(const User *sender, const std::string &new_topic);
		std::string	getTopic() const;
		std::string	getKey() const;
		bool		isChanop(const std::string &nickname) const;
		size_t		getClientLimit() const;

	private:
		std::string				_name;
		std::vector<User *>		_users;

		std::map<char, bool>	_modes;
		std::string				_topic;
		std::string				_key;
		std::map<std::string, bool>	_chanops;
		size_t					_client_limit;
};

#endif
