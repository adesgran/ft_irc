/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 09:08:13 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/20 11:46:01 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Log.hpp>
std::string	_fileName( void )
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	std::stringstream stream;
	stream 
		<< "log/"
		<< ltm->tm_year + 1900 << "-"
		<< std::setfill('0') << std::setw(2) << ltm->tm_mon + 1 << "-"
		<< std::setfill('0') << std::setw(2) << ltm->tm_mday << "_" 
		<< std::setfill('0') << std::setw(2) << ltm->tm_hour << ":"
		<< std::setfill('0') << std::setw(2) << ltm->tm_min << ":"
		<< std::setfill('0') << std::setw(2) << ltm->tm_sec
		<< ".log";
	std::string res;
	stream >> res;
	return (res);
}

std::string	_getDate( void )
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	std::stringstream stream;
	stream 
		<< std::setfill('0') << std::setw(2) << ltm->tm_mon + 1 << "."
		<< std::setfill('0') << std::setw(2) << ltm->tm_mday;
	std::string res;
	stream >> res;
	return (res);
}

std::string	_getHour( void )
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	std::stringstream stream;
	stream 
		<< std::setfill('0') << std::setw(2) << ltm->tm_hour << ":"
		<< std::setfill('0') << std::setw(2) << ltm->tm_min << ":"
		<< std::setfill('0') << std::setw(2) << ltm->tm_sec;
	std::string res;
	stream >> res;
	return (res);
}

Log::Log(void) : _logFile(_fileName())
{
	_logFile = _fileName();
}

Log::Log(const Log &log)
{
	*this = log;
}

Log::~Log(void)
{
};

Log &Log::operator=(const Log &log)
{
	if ( this != &log )
		this->_logFile = log._logFile;
	return (*this);
}

void	Log::debug( std::string str )
{
	std::cout << _getHour() << " \e[33m\[Server]\e[0m " << str << std::endl;
	this->_writeFile("[Server] " + str);
}

void	Log::info( std::string str )
{
	std::cout << _getHour() << " \e[33m\[Server]\e[0m \e[36mINFO\e[0m " << str << std::endl;
	this->_writeFile("[Server] INFO " + str);
}

void	Log::warning( std::string str )
{
	std::cout << _getHour() << " \e[33m\[Server]\e[0m \e[35mWARN\e[0m " << str << std::endl;
	this->_writeFile("[Server] WARN " + str);
}

void	Log::error( std::string str )
{
	std::cout << _getHour() << " \e[33m\[Server]\e[0m \e[91mERROR\e[0m " << str << std::endl;
	this->_writeFile("[Server] ERROR " + str);
}

void	Log::_writeFile( std::string str )
{
	std::ofstream file (this->_logFile.c_str(), std::ofstream::app);
	if ( !file || !file.is_open() )
		return;
	file << _getDate() << " " << _getHour() << " " << str << std::endl;
	file.close();
	return;

}


	
