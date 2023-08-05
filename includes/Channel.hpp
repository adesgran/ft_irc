/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:53:07 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/05 14:11:42 by mchassig         ###   ########.fr       */
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
		Channel( const std::string name );
		Channel( const Channel &channel );
		~Channel( void );
		Channel &operator=( const Channel &channel );

		const std::vector<User *>	getUsers( void ) const;
		void	addUser( User *target, User *sender = NULL);
		void	removeUser( const User *user );
		void	removeUser( int fd );
		bool	isUserOnChannel(const std::string &nickname) const;

		void		setName( const std::string &name );
		std::string	getName( void ) const;

		bool		setModes(const std::string &new_modes, const std::string &mode_arg);
		std::string	getActiveModes() const;
		void		setKey(const User *sender, const std::string key);
		std::string	getKey() const;
		void		setTopic(const User *sender, const std::string &new_topic);
		std::string	getTopic() const;
		void		setClientLimit(const User *sender, size_t new_lim);
		size_t		getClientLimit() const;

		std::string			status;

	private:
		std::vector<User *>		_users;
		std::string				_name;
		std::map<char, bool>	_modes;
		std::string				_key; // set/unset with "k" mode, by chanops only
		std::string				_topic; // limited no chanops if 't' mode is enabled
		size_t					_client_limit; // set/unset with "l" mode, by chanops only
		// std::vector<User *>		_chanops; // nickname with "@" prefix
};

#endif
