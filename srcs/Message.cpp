/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 12:21:22 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/13 12:25:56 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Message.hpp>

Message::Message(void)
{
}

Message::Message(const Message &message)
{
	(void)message;
}

Message::~Message(void)
{
};

Message &Message::operator=(const Message &message)
{
	(void)message;
	return (*this);
}

void	Message::read( std::string message )
{
	std::cout << message << std::endl;
}
