/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:20:36 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/14 17:50:48 by mchassig         ###   ########.fr       */
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
	PASS,
	NICK,
	USER,
	PING,
	JOIN,
	PART,
	TOPIC,
	INVITE,
	KICK,
	MODE,
	PRIVMSG,
	WHOIS,
};

# define USERTAG( u ) u->getNickname() << '!' << u->getUsername() \
		<< "@localhost"

# define FOREACH(name, it) for (std::map<User*, bool>::iterator it = name.begin();\
							it != name.end();\
							it++)

# define CRLF	"\r\n"

# define RPL_WELCOME			"001"
# define RPL_UMODEIS			"221"
# define RPL_AWAY				"301"
# define RPL_ENDOFWHOIS			"318"
# define RPL_CHANNELMODEIS		"324"
# define RPL_NOTOPIC			"331"
# define RPL_TOPIC				"332"
# define RPL_INVITING			"341"
# define RPL_NAMREPLY			"353"
# define RPL_ENDOFNAMES			"366"

# define ERR_NOSUCHNICK			"401"
# define ERR_NOSUCHSERVER		"402"
# define ERR_NOSUCHCHANNEL		"403"
# define ERR_NORECIPIENT		"411"
# define ERR_NOTEXTTOSEND		"412"
# define ERR_UNKNOWNCOMMAND		"421"
# define ERR_NONICKNAMEGIVEN	"431"
# define ERR_ERRONEUSNICKNAME	"432"
# define ERR_NICKNAMEINUSE		"433"
# define ERR_USERNOTINCHANNEL	"441"
# define ERR_NOTONCHANNEL		"442"
# define ERR_USERONCHANNEL		"443"
# define ERR_NOTREGISTERED		"451"
# define ERR_NEEDMOREPARAMS		"461"
# define ERR_ALREADYREGISTERED	"462"
# define ERR_PASSWDMISMATCH		"464"
# define ERR_CHANNELISFULL		"471"
# define ERR_INVITEONLYCHAN		"473"
# define ERR_BADCHANNELKEY		"475"
# define ERR_CHANOPRIVSNEEDED	"482"
# define ERR_UMODEUNKNOWNFLAG	"501"
# define ERR_USERSDONTMATCH		"502"
# define ERR_INVALIDKEY			"525"
# define ERR_INVALIDMODEPARAM	"696"

class Message {
	public:
		Message( void );
		Message( User *sender );
		Message( const Message &message );
		~Message( void );
		Message &operator=( const Message &message );

		void				setInputMsg(const std::string &input_buffer, Server *server);
		std::string			getInputMsg() const;
		std::string			getOutputMsg();
		void				addNumericMsg(const std::string code, const std::string arg = "");
		void				addMsg(const User *source, const std::string cmd, const std::string target, const std::string arg = "");

		// Exception ------------------------------------------
		class NumericReply : public std::exception
		{
			public:
				NumericReply(std::string code, std::string param) throw();
				virtual ~NumericReply(void) throw();

				virtual const char *code() const throw();
				virtual const char *what() const throw();

			private:
				std::string	_code;
				std::string	_what;
		};

	private:
		typedef void (Message::*cmdFunction)(const std::string&);
		std::map<std::string, cmdFunction>	_cmdMap;
		User*								_sender;
		Server*								_server;

		std::string							_input;
		std::stringstream					_output;

		// Utils ----------------------------------------------
		void						_parseInput(const std::vector<std::string> &input_lines);
		std::vector<std::string>	_split(const std::string &str, const std::string &sep) const;

		// IRC commands -----------------------------------------
		void	_welcomeNewUser();

		void	_pass(const std::string &arg);
		void	_nick(const std::string &arg);
		void	_user(const std::string &arg);
		void	_pong(const std::string &arg);
		void	_join(const std::string &arg);
		void	_part(const std::string &arg);
		void	_topic(const std::string &arg);
		void	_invite(const std::string &arg);
		void	_kick(const std::string &arg);
		void	_mode(const std::string &arg);
		void	_privmsg(const std::string &arg);
		void	_whois(const std::string &arg);
};

#endif
