/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchassig <mchassig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 12:59:54 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/15 16:56:28 by adesgran         ###   ########.fr       */
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
	if (name.size() < 2 || name[0] != '#')
		throw Message::NumericReply("448", name + " :Cannot join channel: Channel name must start with a hash mark (#)");
	_name = name;
	_members.insert(std::make_pair(founder, true));
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
	this->_name = channel._name;
	this->_members = channel._members;
	this->_modes = channel._modes;
	this->_modes_diff = channel._modes_diff;
	this->_modes_diff_arg = channel._modes_diff_arg;
	this->_topic = channel._topic;
	this->_key = channel._key;
	this->_client_limit = channel._client_limit;
	return (*this);
}

const std::map<User*,bool>	Channel::getMembers( void ) const
{
	return (this->_members);
}

void	Channel::addMember( User *target, User *sender, const std::string &key )
{
	if (_modes['i'] && sender == NULL)
		throw Message::NumericReply(ERR_INVITEONLYCHAN, _name + " :Cannot join channel (+i)");
	if (_modes['i'] && !isChanop(sender->getNickname()))
		throw Message::NumericReply(ERR_CHANOPRIVSNEEDED, _name + " :You're not channel operator");
	if (!_modes['i'] && _modes['k'] && _key.compare(key))
		throw Message::NumericReply(ERR_BADCHANNELKEY, _name + " :Cannot join channel (+k)");
	if (isMember(target->getNickname()))
		throw Message::NumericReply(ERR_USERONCHANNEL, target->getNickname() + " " + _name + ":is already on channel");
	if (_modes['l'] && _members.size() == _client_limit)
		throw Message::NumericReply(ERR_CHANNELISFULL, _name + ":Cannot join channel (+l)");
	_members.insert(std::make_pair(target, false));
}

void	Channel::removeMember( User *target )
{
	std::map<User*, bool>::iterator	to_remove = _members.find(target);
	if (to_remove == _members.end())
		return ;
	_members.erase(to_remove);
	if (_members.size() == 0)
		return ;
	size_t	nb_chanop = 0;
	FOREACH(_members, it)
		if (it->second)
			nb_chanop++;
	if (nb_chanop == 0)
	{
		std::map<User*, bool>::iterator	new_chanop = _members.begin();
		new_chanop->second = true;
		FOREACH(_members, it)
			it->first->getMessage()->addReply(target, "MODE", _name, ":+o " + new_chanop->first->getNickname());
	}
}

/*
void	Channel::removeMember( int fd )
{
	std::map<User*, bool>::iterator	to_remove;
	for ( std::map<User*, bool>::iterator it = this->_members.begin();
			it != this->_members.end();
			it++)
	{
		if ( it->first->getSockfd() == fd )
		{
			to_remove = it;
			break ;
		}
	}
	//
	User target(to_remove->first);
	std::cout << "SIZE = " << _members.size() << std::endl;
	if (to_remove == _members.end())
		return ;
	_members.erase(to_remove);
	if (_members.size() == 0)
		return ;
	size_t	nb_chanop = 0;
	FOREACH(_members, it)
		if (it->second)
			nb_chanop++;
	if (nb_chanop == 0)
	{
		std::map<User*, bool>::iterator	new_chanop = _members.begin();
		if (!new_chanop->second)
			new_chanop->second = true;
		FOREACH(_members, it)
			it->first->getMessage()->addReply(&target, "MODE", _name, ":+o " + new_chanop->first->getNickname());
	}
}
*/

bool	Channel::isMember(const std::string &nickname) const
{
	for (std::map<User*, bool>::const_iterator it = _members.begin(); it != _members.end(); it++)
	{
		if (isEquals(nickname, it->first->getNickname()))
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
	std::vector<std::string>	new_chanops, old_chanops;
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
							sender->getMessage()->addReply(sender, ERR_INVALIDMODEPARAM, sender->getNickname(), _name + " " + *it + " :Missing key");
						else if (new_key.find_first_of(" ,") != std::string::npos)
							sender->getMessage()->addReply(sender, ERR_INVALIDKEY, sender->getNickname(), _name + " :Key is not well-formed");						
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
						sender->getMessage()->addReply(sender, ERR_INVALIDMODEPARAM, sender->getNickname(), _name + " " + *it + " :Missing user nickname");
					else if (!isMember(nickname))
						sender->getMessage()->addReply(sender, ERR_USERNOTINCHANNEL, sender->getNickname(), _name + " " + *it + " " + nickname + " :User is not on channel");
					else
					{
						if (sign == '+')
							new_chanops.push_back(nickname);
						else
							old_chanops.push_back(nickname);
					}
					break;
				}
				case 'l': {
					if (sign == '+')
					{
						std::string	tmp;
						if (!std::getline(ss, tmp, ' ') || tmp.empty())
							sender->getMessage()->addReply(sender, ERR_INVALIDMODEPARAM, sender->getNickname(), _name + " " + *it + " :Missing client limit");
						else if (atoi(tmp.c_str()) == 0)
							sender->getMessage()->addReply(sender, ERR_INVALIDMODEPARAM, sender->getNickname(), _name + " " + *it + " :Client limit cannot be 0");
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
	for (std::vector<std::string>::iterator it = new_chanops.begin(); it!= new_chanops.end(); it++)
	{
		std::vector<std::string>::iterator	tmp = std::find(old_chanops.begin(), old_chanops.end(), *it);
		if ( tmp != old_chanops.end())
		{
			new_chanops.erase(it);
			old_chanops.erase(tmp);
		}
	}
	_setModesDiff(changes, new_chanops, old_chanops);
	_modes = changes;
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

void	Channel::setTopic(User *sender, const std::string &new_topic)
{
	if (_modes['t'] && !isChanop(sender->getNickname()))
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

void	Channel::setChanop(const std::string &nickname, bool status)
{
	if (!isMember(nickname))
		return ;
	for (std::map<User*, bool>::iterator it = _members.begin(); it != _members.end(); it++)
	{
		if (isEquals(nickname, it->first->getNickname()))
		{
			it->second = status;
			return ;
		}
	}
}

bool	Channel::isChanop(const std::string &nickname) const
{
	for (std::map<User*, bool>::const_iterator it = _members.begin(); it != _members.end(); it++)
	{
		if (isEquals(nickname, it->first->getNickname()))
			return (it->second);
	}
	return (false);
}

size_t	Channel::getClientLimit() const
{
	return (_client_limit);
}

void	Channel::_setModesDiff( std::map<char, bool> &changes, std::vector<std::string> &new_chanops, std::vector<std::string> &old_chanops)
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
	for (std::vector<std::string>::iterator it = new_chanops.begin(); it != new_chanops.end(); it++)
	{
		if (isMember(*it) && !isChanop(*it))
		{
			_modes_diff += "o";
			_modes_diff_arg += *it + " ";
			setChanop(*it, true);
		}
	}
	if (*(_modes_diff.end() - 1) == '+')
		_modes_diff.erase(_modes_diff.end()-1, _modes_diff.end());
	_modes_diff += "-";
	for (std::map<char, bool>::iterator it = _modes.begin(); it != _modes.end(); it++)
	{
		if ((*it).second != changes[(*it).first] && (*it).second == true)
			_modes_diff += (*it).first;
	}
	for (std::vector<std::string>::iterator it = old_chanops.begin(); it != old_chanops.end(); it++)
	{
		if (isMember(*it) && isChanop(*it))
		{
			_modes_diff += "o";
			_modes_diff_arg += *it + " ";
			setChanop(*it, false);
		}
	}
	if (*(_modes_diff.end() - 1) == '-')
		_modes_diff.erase(_modes_diff.end()-1, _modes_diff.end());
	if (!_modes_diff_arg.empty())
		_modes_diff_arg.erase(_modes_diff_arg.end()-1, _modes_diff_arg.end());
}
