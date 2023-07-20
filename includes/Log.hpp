/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 09:07:27 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/20 11:40:04 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
# define LOG_HPP

# include <iostream>
# include <string>
# include <ctime>
# include <fstream>
# include <sstream>
# include <iomanip>

class Log {
	public:
		Log( void );
		Log( const Log &log );
		~Log( void );
		Log &operator=( const Log &log );

		void	debug( std::string str );
		void	info( std::string str );
		void	warning( std::string str );
		void	error( std::string str );

		void	_writeFile( std::string str );


	protected:
		std::string _logFile;

};

#endif
