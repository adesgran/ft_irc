/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 17:39:46 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/01 17:53:56 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils.hpp>

std::string	hostname_to_ip( std::string hostname )
{
	struct hostent	*hostent;
	struct in_addr	**addr_list;

	if ( (hostent = gethostbyname( hostname.c_str() ) ) == NULL )
	{
		std::cerr << "Error 'gethostbyname'" << std::endl;
		herror("gethostbyname");
		return ("");
	}

	addr_list = (struct in_addr **)hostent->h_addr_list;

	if (!addr_list[0])
	{
		std::cerr << "Address does not exist" << std::endl;
		return ("");
	}
	return (inet_ntoa(*addr_list[0]));
}
