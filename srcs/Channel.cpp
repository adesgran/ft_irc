/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:59:54 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/14 15:10:25 by mchassig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

Channel::Channel(void)
{
	this->_name = "";
	_modes['i'] = false;
	_modes['t'] = false;
	_modes['k'] = false;
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
	if (_modes['i'] && sender == NULL)
		throw Message::NumericReply(ERR_INVITEONLYCHAN, _name + " :Cannot join channel (+i)");
	if (_modes['i'] && !_chanops[sender->getNickname()])
		throw Message::NumericReply(ERR_CHANOPRIVSNEEDED, _name + " :You're not channel operator");
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
		if (isEquals(nickname, (*it)->getNickname()))
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
	std::map<char, bool>	changes(_modes);
	std::map<std::string, bool>	changes_chanops(_chanops);
	std::string	new_key;
	size_t		new_lim = 0;
	for (std::string::const_iterator it = modestring.begin(); it != modestring.end(); it++)
	{
		if (*it == '+' || *it == '-')
			sign = *it;
		else if(sign != 0 && (_modes.find(*it) != _modes.end() || *it == 'o'))
		{
			switch (*it)
			{
				case 'i' : {
					if (sign == '+')
						changes['i'] = true;
					else
						changes['i'] = false;
					break;					
				}
				case 't' : {
					if (sign == '+')
						changes['t'] = true;
					else
						changes['t'] = false;
					break;
				}
				case 'k': {
					if (sign == '+')
					{
						if (!std::getline(ss, new_key, ' ') || new_key.empty())
							sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " :Missing key");
						else if (new_key.find_first_of(" ,") != std::string::npos)
							sender->getMessage()->addNumericMsg(ERR_INVALIDKEY, _name + " :Key is not well-formed");						
						else
							changes['k'] = true;
					}
					else
						changes['k'] = false;
					break;
				}
				case 'o': {
					std::string	nickname;
					if (!std::getline(ss, nickname, ' ') || nickname.empty())
						sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " :Missing user nickname");
					else if (!isUserOnChannel(nickname))
						sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " " + nickname + " :User is not on channel");
					else
					{
						if (sign == '+')
							changes_chanops[nickname] = true;
						else
							changes_chanops[nickname] = false;
					}
					break;
				}
				case 'l': {
					if (sign == '+')
					{
						std::string	tmp;
						if (!std::getline(ss, tmp, ' ') || tmp.empty())
							sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " :Missing client limit");
						else if (atoi(tmp.c_str()) == 0)
							sender->getMessage()->addNumericMsg(ERR_INVALIDMODEPARAM, _name + " " + *it + " :Client limit cannot be 0");
						else
						{
							new_lim = atoi(tmp.c_str());
							changes['l'] = true;
						}
					}
					else
						changes['l'] = false;
					break;
				}
				default:
					break;
			}
		}
	}
	if (!new_key.empty() && changes['k'] && new_key.compare(_key))
	{
		_modes['k'] = false;
		_key = new_key;
	}
	if (new_lim != 0 && changes['l'] && new_lim != _client_limit)
	{
		_modes['l'] = false;
		_client_limit = new_lim;
	}
	_setModesDiff(changes, changes_chanops);
	_modes = changes;
	_chanops = changes_chanops;
	return (!_modes_diff.empty());
}

std::string	Channel::getModesDiff( void ) const
{
	return (_modes_diff);
}

std::string	Channel::getModesDiffArg( void ) const
{
	return (_modes_diff_arg);
}

std::string	Channel::getActiveModes()
{
	_modes_diff_arg.clear();
	std::string ret;
	if (_modes['i'])
	{
		ret += 'i';
	}
	if (_modes['t'])
	{
		ret += 't';
	}
	if (_modes['k'])
	{
		ret += 'k';
		_modes_diff_arg += _key + " ";
	}
	if (_modes['l'])
	{
		ret += 'l';
		std::stringstream	ss;
		ss << _client_limit;
		_modes_diff_arg += ss.str() + " ";			
	}
	if (!_modes_diff_arg.empty())
		_modes_diff_arg.erase(_modes_diff_arg.end()-1, _modes_diff_arg.end());
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

void	Channel::_setModesDiff( std::map<char, bool> &changes, std::map<std::string, bool> &changes_chanops)
{
	_modes_diff.clear();
	_modes_diff_arg.clear();
	_modes_diff = "+";
	for (std::map<char, bool>::iterator it = _modes.begin(); it != _modes.end(); it++)
	{
		if ((*it).second != changes[(*it).first] && (*it).second == false)
		{
			_modes_diff += (*it).first;
			if ((*it).first == 'k')
				_modes_diff_arg += _key + " ";
			else if ((*it).first == 'l')
			{
				std::stringstream	ss;
				ss << _client_limit;
				_modes_diff_arg += ss.str() + " ";	
			}
		}
	}
	std::cout << "+diff:" << _modes_diff << "|arg:" << _modes_diff_arg << "|\n";
	for (std::map<std::string, bool>::iterator it = _chanops.begin(); it != _chanops.end(); it++)
	{
		
		if ((*it).second != changes_chanops[(*it).first] && (*it).second == false)
		{
			_modes_diff += "o";
			_modes_diff_arg += (*it).first + " ";
		}
	}
	std::cout << "+diff:" << _modes_diff << "|arg:" << _modes_diff_arg << "|\n";
	if (*(_modes_diff.end() - 1) == '+')
		_modes_diff.erase(_modes_diff.end()-1, _modes_diff.end());
	_modes_diff += "-";
	for (std::map<char, bool>::iterator it = _modes.begin(); it != _modes.end(); it++)
	{
		if ((*it).second != changes[(*it).first] && (*it).second == true)
			_modes_diff += (*it).first;
	}
	std::cout << "-diff:" << _modes_diff << "|arg:" << _modes_diff_arg << "|\n";
	for (std::map<std::string, bool>::iterator it = _chanops.begin(); it != _chanops.end(); it++)
	{
		
		if ((*it).second != changes_chanops[(*it).first] && (*it).second == true)
		{
			_modes_diff += "o";
			_modes_diff_arg += (*it).first + " ";
		}
	}
	std::cout << "-diff:" << _modes_diff << "|arg:" << _modes_diff_arg << "|\n";
	if (*(_modes_diff.end() - 1) == '-')
		_modes_diff.erase(_modes_diff.end()-1, _modes_diff.end());
	if (!_modes_diff_arg.empty())
		_modes_diff_arg.erase(_modes_diff_arg.end()-1, _modes_diff_arg.end());
}
