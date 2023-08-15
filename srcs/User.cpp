/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/01 13:44:20 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/15 15:16:28 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <User.hpp>

User::User(void)
{
	this->_sockfd = 1;
	this->_isop = false;
	this->_welcomed = false;
	this->_message = new Message(this);
	_modes['i'] = false;
	_modes['w'] = false;
	_modes['o'] = false;
	_modes['O'] = false;
}

User::User(int sockfd)
{
	this->_sockfd = sockfd;
	this->_isop = false;
	this->_welcomed = false;
	this->_message = new Message(this);
	_modes['i'] = false;
	_modes['w'] = false;
	_modes['o'] = false;
	_modes['O'] = false;
}

User::User(const User &user)
{
	this->_message = NULL;
	*this = user;
}

User::~User(void)
{
	delete this->_message;
};

User &User::operator=(const User &user)
{
	if (this == &user)
		return (*this);
	if (this->_message != NULL)
		delete this->_message;
	this->_sockfd = user._sockfd;
	this->_isop = user._isop;
	this->_authenticated = user._authenticated;
	this->_username = user._username;
	this->_nickname = user._nickname;
	this->_realname = user._realname;
	this->_hostname = user._hostname;
	this->_modes = user._modes;
	this->_modes_diff = user._modes_diff;
	this->_message = new Message(*(user._message));
	this->_welcomed = user._welcomed;
	return (*this);
}

void	User::setSockfd( int sockfd )
{
	this->_sockfd = sockfd;
}

int	User::getSockfd( void ) const
{
	return (this->_sockfd);
}

void	User::setIsop( bool isop )
{
	this->_isop = isop;
}

bool	User::getIsop( void ) const
{
	return (this->_isop);
}

bool	User::isAuthenticated() const
{
	return (_authenticated);
}

void	User::setAuthenticated(bool status)
{
	_authenticated = status;
}

std::string	User::getUsername( void ) const
{
	return (this->_username);
}

void	User::setUsername( const std::string username )
{
	this->_username = username;
}

std::string	User::getNickname( void ) const
{
	return (this->_nickname);
}

void	User::setNickname( const std::string nickname )
{
	this->_nickname = nickname;
}

std::string	User::getRealname( void ) const
{
	return (this->_realname);
}

void	User::setRealname( const std::string realname )
{
	this->_realname = realname;
}

std::string	User::getHostname( void ) const
{
	return (this->_hostname);
}

void	User::setHostname( const std::string hostname )
{
	this->_hostname = hostname;
}

std::string	User::getActiveModes( void ) const
{
	std::string ret;
	if (_modes.at('i'))
		ret += 'i';
	if (_modes.at('w'))
		ret += 'w';
	if (_modes.at('o'))
		ret += 'o';
	if (_modes.at('O'))
		ret += 'O';
	return (ret);
}

bool	User::setModes(const std::string modestring)
{
	char	op = 0;
	std::map<char, bool>	changes(_modes);
	for (std::string::const_iterator it = modestring.begin(); it != modestring.end(); it++)
	{
		if (*it == '+' || *it == '-')
			op = *it;
		else if (op == '+' && _modes.find(*it) != _modes.end() && *it != 'o' && *it != 'O')
			changes[*it] = true;
		else if (op == '-' && _modes.find(*it) != _modes.end())
			changes[*it] = false;
		else if (*it != 'o' && *it != 'O')
			_message->addReply(this, ERR_UMODEUNKNOWNFLAG, _nickname,":Unknown MODE flag");
	}
	_setModesDiff(changes);
	_modes = changes;
	return (!_modes_diff.empty());
}

std::string	User::getModesDiff( void ) const
{
	return (_modes_diff);
}

Message	*User::getMessage( void ) const
{
	return (_message);
}

bool	User::isWelcomed( void ) const
{
	return (_welcomed);
}

void		User::welcome( void )
{
	_welcomed = true;
}

void	User::_setModesDiff( std::map<char, bool> &changes)
{
	_modes_diff = "+";
	for (std::map<char, bool>::iterator it = _modes.begin(); it != _modes.end(); it++)
		if ((*it).second != changes[(*it).first] && (*it).second == false)
			_modes_diff += (*it).first;
	if (*(_modes_diff.end() - 1) == '+')
		_modes_diff.erase(_modes_diff.end()-1, _modes_diff.end());
	_modes_diff += "-";
	for (std::map<char, bool>::iterator it = _modes.begin(); it != _modes.end(); it++)
		if ((*it).second != changes[(*it).first] && (*it).second == true)
			_modes_diff += (*it).first;
	if (*(_modes_diff.end() - 1) == '-')
		_modes_diff.erase(_modes_diff.end()-1, _modes_diff.end());
}
