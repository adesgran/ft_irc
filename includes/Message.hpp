/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:20:36 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/04 17:05:12 by mchassig         ###   ########.fr       */
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

/*
 # define ITOA( x ) static_cast< std::ostringstream & >( \
		 ( std::ostringstream() << std::dec << x ) ).str()
		 */

# define USERTAG( u ) u->getNickname() << '!' << u->getUsername() \
		<< "@localhost"

# define RPL_WELCOME			"001"
# define RPL_UMODEIS			"221"
# define RPL_AWAY				"301"
# define RPL_ENDOFWHOIS			"318"
# define RPL_CHANNELMODEIS		"324"
# define RPL_TOPIC				"332"
# define RPL_NAMREPLY			"353"
# define RPL_ENDOFNAMES			"366"

# define ERR_NOSUCHNICK			"401"
# define ERR_NOSUCHSERVER		"402"
# define ERR_NOSUCHCHANNEL		"403"
# define ERR_NORECIPIENT		"411"
# define ERR_NOTEXTTOSEND		"412"
# define ERR_NONICKNAMEGIVEN	"431"
# define ERR_ERRONEUSNICKNAME	"432"
# define ERR_NICKNAMEINUSE		"433"
# define ERR_USERNOTINCHANNEL	"441"
# define ERR_NOTONCHANNEL		"442"
# define ERR_NEEDMOREPARAMS		"461"
# define ERR_ALREADYREGISTERED	"462"
# define ERR_BADCHANNELKEY		"475"
# define ERR_CHANOPRIVSNEEDED	"482"
# define ERR_UMODEUNKNOWNFLAG	"501"
# define ERR_USERSDONTMATCH		"502"
# define ERR_INVALIDMODEPARAM	"696"

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
		void				appendOutputMsg(std::string err_code, std::string arg = "");

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
		void	_welcomeNewUser();

		void	_nick(std::string arg);
		void	_user(std::string arg);
		void	_join(std::string arg);
		void	_privmsg(std::string arg);
		void	_kick(std::string arg);
		void	_invite(std::string arg);
		void	_topic(std::string arg);
		void	_mode(std::string arg);
		void	_ping(std::string arg);
		void	_whois(std::string arg);

		// // Exception ------------------------------------------
		// class CommandErrorException : public std::exception
		// {
		// 	public:
		// 		CommandErrorException(int codeError, std::string arg) throw();
		// 		virtual ~CommandErrorException(void) throw();

		// 		virtual const char *what() const throw();

		// 	private:
		// 		std::string	msg;
		// };

};

#endif
