/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:20:36 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/01 08:46:07 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <string>
# include <map>
# include <sstream>
# include <vector>
# include <User.hpp>
# include <Server.hpp>

# define SERVER_ADDRESS "localhost"

class User;
class Server;

enum cmdValue {
	ERR_UNKNOWNCMD,
	NICK,
	USER,
	JOIN,
	PRIVMSG,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	PING,
	WHOIS,
	CAP,
};

# define ITOA( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

# define USERTAG( u ) u->getNickname() << '!' << u->getUsername() \
		<< "@localhost"

# define RPL_UMODEIS			221
# define RPL_ENDOFWHOIS			318

# define ERR_NOSUCHNICK			401
# define ERR_NOSUCHSERVER		402
# define ERR_NONICKNAMEGIVEN	431
# define ERR_ERRONEUSNICKNAME	432
# define ERR_NICKNAMEINUSE		433
# define ERR_NEEDMOREPARAMS		461
# define ERR_ALREADYREGISTERED	462
# define ERR_UMODEUNKNOWNFLAG	501
# define ERR_USERSDONTMATCH		502

class Message {
	public:
		Message( void );
		Message( User *sender );
		Message( const Message &message );
		~Message( void );
		Message &operator=( const Message &message );

		void				setInputMsg(std::string &input_buffer, Server *server);
		std::string			getInputMsg() const;
		std::string			getOutputMsg() ;

	private:
		std::map<std::string, cmdValue>	_cmdMap; //maybe change cmdValue to a pointer to function
		User*							_sender;
		Server*							_server;

		std::string						_input;
		std::stringstream				_output;

		// Utils ----------------------------------------------
		void						_parseInput(std::vector<std::string> input_lines);
		std::vector<std::string>	_split(const std::string &str, const std::string &sep) const;

		// IRC commands -----------------------------------------
		void	_nick(std::vector<std::string> arg);
		void	_user(std::vector<std::string> arg);
		void	_join(std::vector<std::string> arg);
		void	_privmsg(std::vector<std::string> arg);
		void	_kick(std::vector<std::string> arg);
		void	_invite(std::vector<std::string> arg);
		void	_topic(std::vector<std::string> arg);
		void	_mode(std::vector<std::string> arg);
		void	_ping(std::vector<std::string> arg);
		void	_whois(std::vector<std::string> arg);
};

#endif
