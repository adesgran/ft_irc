/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/01 15:36:55 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/02 11:58:41 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# include <User.hpp>
# include <Channel.hpp>
# include <Server.hpp>

void	read_msg( std::string &msg, User &user, Server &server );

#endif
