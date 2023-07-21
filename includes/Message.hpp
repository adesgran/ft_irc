/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:20:36 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/21 14:08:17 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <string>
# include <User.hpp>

enum cmdValue {
	ERR_UNKNOWNCMD,
	CAP,
	NICK,
	USER,
	JOIN,
	PRIVMSG,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	PING,
};

class User;

class Message {
	public:
		Message( void );
		Message( const Message &message );
		~Message( void );
		Message &operator=( const Message &message );

		void	read( std::string message );

	private:
		cmdValue						_cmdName;
		std::map<std::string, cmdValue>	_cmdMap;

		// sender
		// receiver
		// payload
		// command called
		// [...]

		// Parsing ----------------------------------------------
		void						_cmdparser(std::string cmdline, User &user);
		std::vector<std::string>	_split(const std::string& str, char sep);
		
		// IRC commands -----------------------------------------
		void	_cmdInit();
		void	_cap(std::vector<std::string> arg, User &user);
		void	_nick(std::vector<std::string> arg, User &user);
		void	_user(std::vector<std::string> arg, User &user);
		// PRIVMSG
		// KICK
		// INVITE
		// TOPIC
		// MODE
		// PING
		


};

#endif
