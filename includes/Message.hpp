/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:20:36 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/21 18:29:46 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <User.hpp>

class User;

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
	CAP,
};

class Message {
	public:
		Message( void );
		Message( std::string buffer, User *sender );
		Message( const Message &message );
		~Message( void );
		Message &operator=( const Message &message );

		// void	read( std::string message );

		void		setInputMsg(std::string &input_buffer);
		std::string	getInputMsg() const;
		std::string	getOutputMsg() const;

		// IRC commands -----------------------------------------
		void	nick(std::vector<std::string> arg);
		void	user(std::vector<std::string> arg);
		void	privmsg(std::vector<std::string> arg);
		void	kick(std::vector<std::string> arg);
		void	invite(std::vector<std::string> arg);
		void	topic(std::vector<std::string> arg);
		void	mode(std::vector<std::string> arg);
		void	ping(std::vector<std::string> arg);

	private:
		std::map<std::string, cmdValue>	_cmdMap; //maybe change cmdValue to a pointer to function
		User*							_sender;
		
		std::string						_input;
		std::string						_output;

		// Utils ----------------------------------------------
		void						_parseInput(std::vector<std::string> input_lines);
		std::vector<std::string>	_split(const std::string& str, char sep);
		
};

#endif
