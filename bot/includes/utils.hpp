/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 17:40:06 by adesgran          #+#    #+#             */
/*   Updated: 2023/08/01 17:53:47 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <string>
# include <stdlib.h>
# include <sys/socket.h>
# include <errno.h>
# include <netdb.h>
# include <arpa/inet.h>


std::string	hostname_to_ip( std::string hostname );

#endif
