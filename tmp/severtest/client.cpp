/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/24 16:56:19 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/13 12:42:43 by adesgran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 6697


int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "PASS 123\n";//USER bbbbbruh 8 * :Bubble\nNICK bruh\n";
    char buffer[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "46.101.34.200", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status
         = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(client_fd, hello, strlen(hello), 0);
    printf("%s\n", hello);
	while (1)
	{
		buffer[0] = '\0';
    	valread = recv(client_fd, buffer, 1024, MSG_DONTWAIT);
		if (buffer[0])
		{
			std::cout << "buffer not empty" << std::endl;
			//buffer[valread] = '\0';
   			printf("%s", buffer);
		}
	}

    // closing the connected socket
    close(client_fd);
    return 0;
}

/*
int main( void )
{

	struct sockaddr addr;
	socklen_t	addrlen;
	int	sockfd;

	if ( (sockfd  = socket(AF_LOCAL, SOCK_STREAM, 0)) < 1)
	{
		std::cout << "Error socket()" << std::endl;
		return 1;
	}
	addrlen = sizeof(addr);

	if ( getsockname(sockfd, &addr, &addrlen) )
	{
		std::cout << "Error getsockname()" << std::endl;
		return 1;
	}



	close(sockfd);

	return 0;
}

*/