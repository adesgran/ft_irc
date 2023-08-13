/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 16:50:23 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/13 02:29:00 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Bot.hpp>

Bot::Bot(void)
{
	this->_logged = 0;
	this->_opt = 1;
	this->_addrlen = sizeof(this->_sockaddr);

	if ( (this->_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		std::cerr << "Error on socket creation" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}

	if ( setsockopt(
				this->_fd,
				SOL_SOCKET,
				SO_REUSEADDR,
				&this->_opt,
				sizeof(this->_opt) ) )
	{
		std::cerr << "Error on socket options" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}



}

Bot::Bot(const Bot &bot)
{
	*this = bot;
}

Bot::~Bot(void)
{
};

Bot &Bot::operator=(const Bot &bot)
{
	if ( this == &bot )
		return *this;
	return *this;

}

void	Bot::setCmd(std::string const &filename)
{
	std::ifstream file;
	file.open(filename.c_str());
	std::string line;
	while (std::getline(file, line))
	{
		std::string key = line.substr(0, line.find(' '));
		std::string value = line.substr(line.find(' ') + 1);
		_cmd[key] = value;
	}
}

void	Bot::setPass(std::string const &pass)
{
	_pass = pass;
}

void	Bot::setName(std::string const &name)
{
	_name = name;
}

void	Bot::_readline( std::string str )
{
	std::string line(str);
	std::vector<std::string> tokens;
	while (line.size())
	{
		size_t pos = line.find(' ');
		if (pos == std::string::npos)
		{
			tokens.push_back(line);
			break;
		}
		else
		{
			std::string t;
			t = line.substr(0, pos );
			line.erase(0, pos + 1);
			if (pos > 0)
				tokens.push_back(t);
		}
	}

	if (!_logged)
	{
		if ( tokens[1] == "001" )
		{
			_logged = 1;
			std::cout << "Logged on irc server" << std::endl;
			_output << "JOIN #bot\r\n";
		}
		else
		{
			throw std::runtime_error("Can't log in");
		}
	}
	else
	{
		std::string	sender;
		sender = tokens[0].substr(0, tokens[0].find('!'));
		if (sender.size() < 1)
			sender = "#bot";
		if ( tokens[1] == std::string("PRIVMSG") )
		{
			std::string dest;
			if (tokens[2] == _name)
				dest = sender;
			else
				dest = tokens[2];
			std::string key = tokens[3].substr(1);
			key = key.substr(0, key.find('\r'));
			std::string res = _cmd[key];
			if (res.size() > 1)
				_output << "PRIVMSG " << dest << " :" << res << "\r\n";
			else if (key[0] == '!')
				_output << "PRIVMSG " << dest << " :Unknown command\r\n";
			
		}
	}
}

void	Bot::_listenMessage( void )
{
	ssize_t len;
	char	buffer[BUFFER_SIZE];
	buffer[0] = '\0';
	len = recv(_pfds.fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	if (len < 2)
		throw std::runtime_error("Connection Closed");
	buffer[len] = '\0';
	std::stringstream ss;
	ss.str(buffer);
	std::string line;
	while (std::getline(ss, line))
	{
		if (line.size())
		{
			std::cout << "[SERVER] " << line << std::endl;
			_readline(line);
		}
	}
}

void	Bot::_sendMessage( void )
{
	std::string tosend = _output.str();
	if (tosend.size())
		send(_pfds.fd, tosend.c_str(), tosend.size(), 0);
	_output.str("");

}

void	Bot::run( void )
{
	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
	this->_sockaddr.sin_port = htons(6667);//read file
	
	if (inet_pton(AF_INET, hostname_to_ip("localhost").c_str(), &this->_sockaddr.sin_addr) <= 0) //readfile
	{
		std::cerr << "Error while trying to connect" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}

	if (connect(this->_fd, (struct sockaddr*)&_sockaddr, _addrlen) < 0)
	{
		std::cerr << "Connection Fail" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		throw std::runtime_error(strerror(errno));
	}

	if (_pass.size() > 0)
		_output << "PASS " << _pass << "\r\n";
	_output << "NICK " << _name << "\r\n" << "USER " << _name << " " << _name << " irc.adesgran.ovh :fillbot\r\n";
	this->_pfds.fd = this->_fd;
	this->_pfds.events = POLLIN | POLLOUT;

	while (1)
	{
		int ready = poll(&this->_pfds, 1, 1000);
		if ( ready == -1 )
		{
			std::cerr << "Poll return -1" << std::endl;
			return ;
		}
		if ( ready )
		{
			if (_pfds.revents & POLLERR || _pfds.revents & POLLHUP)
			{
				if (_pfds.revents & POLLERR)
				{
					std::cerr << "POLLERR" << std::endl;
					return ;
				}
				else
				{
					std::cout << "Connection closed" << std::endl;
					close(_pfds.fd);
					return ;
				}
			}
			else
			{
				if (_pfds.revents & POLLIN)
					_listenMessage();
				if (_pfds.revents & POLLOUT)
					_sendMessage();
			}
		}
	}
}


int	Bot::conf( std::string filename )
{
	(void)filename;
	return (0);
}



