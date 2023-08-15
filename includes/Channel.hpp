/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:53:07 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/15 16:57:22 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <cstdlib>
# include <algorithm>
# include <vector>
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

		const std::map<User*, bool>	getMembers( void ) const;
		void	addMember( User *target, User *sender = NULL, const std::string &key = "");
		void	removeMember( User *target );
		//void	removeMember( int fd );
		bool	isMember(const std::string &nickname) const;

		void		setName( const std::string &name );
		std::string	getName( void ) const;

		bool		setModes(const User *sender, const std::string &new_modes, std::stringstream &ss);
		std::string	getActiveModes();
		bool		isActiveMode(char c) const;
		std::string	getModesDiff( void ) const;
		std::string	getModesDiffArg( void ) const;
		void		setTopic(User *sender, const std::string &new_topic);
		std::string	getTopic() const;
		std::string	getKey() const;
		void		setChanop(const std::string &nickname, bool status);
		bool		isChanop(const std::string &nickname) const;
		size_t		getClientLimit() const;

	private:
		std::string				_name;
		std::map<User*, bool>	_members;
	
		std::map<char, bool>	_modes;
		std::string				_modes_diff;
		std::string				_modes_diff_arg;
		std::string				_topic;
		std::string				_key;
		size_t					_client_limit;
		
		void		_setModesDiff( std::map<char, bool> &changes, std::vector<std::string> &new_chanops, std::vector<std::string> &old_chanops);
};

#endif
