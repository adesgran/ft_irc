/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:59:54 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/10 13:38:34 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

Channel::Channel(void)
{
	this->_name = "";
	_modes['i'] = false;
	_modes['t'] = false;
	_modes['k'] = false;
	_modes['o'] = false;
	_modes['l'] = false;
}

Channel::Channel( const std::string name, User *founder )
{
	this->_name = name;
	_users.push_back(founder);
	_chanops.insert(std::make_pair(founder->getNickname(), 1));
	_modes['i'] = false;
	_modes['t'] = false;
	_modes['k'] = false;
	_modes['o'] = false;
	_modes['l'] = false;
}

Channel::Channel(const Channel &channel)
{
	*this = channel;
}

Channel::~Channel(void)
{
};

Channel &Channel::operator=(const Channel &channel)
{
	if ( this == &channel )
		return (*this);
	this->_name = channel.getName();
	this->_users = channel._users;
	return (*this);
}


const std::vector<User *>	Channel::getUsers( void ) const
{
	return (this->_users);
}

void	Channel::addUser( User *target, User *sender, const std::string &key )
{
	if (_modes['i'] && (sender == NULL || !_chanops[sender->getNickname()]))
		throw Message::NumericReply(ERR_INVITEONLYCHAN, _name + " :Cannot join channel (+i)");
	if (!_modes['i'] && _modes['k'] && _key.compare(key))
		throw Message::NumericReply(ERR_BADCHANNELKEY, _name + " :Cannot join channel (+k)");
	if (isUserOnChannel(target->getNickname()))
		throw Message::NumericReply(ERR_USERONCHANNEL, target->getNickname() + " " + _name + ":is already on channel");
	if (_modes['l'] && _users.size() == _client_limit)
		throw Message::NumericReply(ERR_CHANNELISFULL, _name + ":Cannot join channel (+l)");
	_users.push_back(target);
	_chanops.insert(std::make_pair(target->getNickname(), 0));
}

void	Channel::removeUser( const User *user )
{
	for ( std::vector<User *>::iterator it = this->_users.begin();
			it != this->_users.end();
			it++)
	{
		if ( *it == user )
		{
			_chanops.erase(_chanops.find((*it)->getNickname()));
			this->_users.erase(it);
			return ;
		}
	}
}

void	Channel::removeUser( int fd )
{
	for ( std::vector<User *>::iterator it = this->_users.begin();
			it != this->_users.end();
			it++)
	{
		if ( (*it)->getSockfd() == fd )
		{
			_chanops.erase(_chanops.find((*it)->getNickname()));
			this->_users.erase(it);
			return ;
		}
	}
}

bool	Channel::isUserOnChannel(const std::string &nickname) const
{
	for (std::vector<User *>::const_iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (!nickname.compare((*it)->getNickname()))
			return (true);
	}
	return (false);
}

void	Channel::setName( const std::string &name )
{
	this->_name = name;
}

std::string	Channel::getName( void ) const
{
	return (this->_name);
}

bool	Channel::setModes(const User *sender, const std::string &modestring, std::stringstream &ss)
{
	char	sign = 0;
	bool	err = false;
	std::vector<std::string>	changed_modes;
	for (std::string::const_iterator it = modestring.begin(); it != modestring.end(); it++)
	{
		if (*it == '+' || *it == '-')
		{
			sign = *it;
		}
		else if((sign == '+' || sign == '-') && _modes.find(*it) != _modes.end())
		{
			bool	b;
			if (sign == '+')
				b = true;
			else
				b = false;
			switch (*it)
			{
				case 'i': {
					_modes['i'] = b;
					break;
				}
				case 't': {
					_modes['t'] = b;
					break;
				}
				case 'k': {
					_modes['k'] = b;
					if (_modes['k'])
					{
						std::string	new_key;
						if (!std::getline(ss, new_key, ','))
							sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " :Missing key");
						else if (new_key.find_first_of(" ,") != std::string::npos)
							sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " " + new_key + " :Not a valid key");						
						else
							_key = new_key;
					}
					break;
				}
				case 'o': {
					std::string	nickname;
					if (!std::getline(ss, nickname, ','))
						sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " :Missing user nickname");
					else if (!isUserOnChannel(nickname))
						sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " " + nickname + " :User is not on channel");
					else
						_chanops[nickname] = b;
					break;
				}
				case 'l': {
					_modes['l'] = b;
					if (_modes['l'])
					{
						std::string	new_lim;
						if (!std::getline(ss, new_lim, ','))
							sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " :Missing client limit");
						else
						{
							std::stringstream tmpss(new_lim);
							tmpss >> _client_limit;
						}
					}
					break;
				}
				default:
					break;
			}
		}
		else
		{
			err = true;
		}
	}
	return (err);
}

std::string	Channel::getActiveModes() const
{
	std::string ret;
	if (_modes.at('i'))
		ret += 'i';
	if (_modes.at('t'))
		ret += 't';
	if (_modes.at('k'))
		ret += 'k';
	if (_modes.at('o'))
		ret += 'o';
	if (_modes.at('l'))
		ret += 'l';
	return (ret);
}

bool	Channel::isActiveMode(char c) const
{
	return (_modes.at(c));
}

void	Channel::setTopic(const User *sender, const std::string &new_topic)
{
	if (_modes['t'] && !_chanops[sender->getNickname()])
		throw Message::NumericReply(ERR_CHANOPRIVSNEEDED, _name + " :You're not channel operator");
	_topic = new_topic;
}

std::string	Channel::getTopic() const
{
	return (_topic);
}


std::string	Channel::getKey() const
{
	return (_key);
}

bool	Channel::isChanop(const std::string &nickname) const
{
	return (_chanops.find(nickname)->second);
}

size_t	Channel::getClientLimit() const
{
	return (_client_limit);
}
