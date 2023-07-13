/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesgran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/24 16:56:03 by adesgran          #+#    #+#             */
/*   Updated: 2023/07/13 13:51:53 by adesgran         ###   ########.fr       */
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
#include <fcntl.h>

#define PORT 6667 

int main(int argc, char const* argv[])
{
	(void)argc;
	(void)argv;
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[5000] = { 0 };

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
	
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	std::cout << "Server on" << std::endl;
	std::cout << "Waiting for connection....." << std::endl;
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
	std::cout << "New connection set !" << std::endl;
	//int flags = fcntl(0, F_GETFL, 0);
//fcntl(0, F_SETFL, flags | O_NONBLOCK);
	while (1)
	{
		std::string msg;
		std::cout << "SERVER >> ";
		if (std::getline(std::cin, msg))
		{
			if (msg.size() > 1)
			{
				msg.push_back('\n');
    			send(new_socket, msg.c_str(), msg.size(), 0);
			}
		}
		buffer[0] = '\0';
        valread = recv(new_socket, buffer, 5000,MSG_DONTWAIT);
		buffer[valread] = '\0';
		if (buffer[0])
		{
			std::string res(buffer);
			while (res.size())
			{
				size_t pos = res.find('\n');
				std::string tmp = res.substr(0, pos);
				std::cout << "CLIENT << " << tmp << std::endl;
				res.erase(0, pos + 1);
				if (tmp.find("PING") != std::string::npos){
					std::string s(tmp.begin() + 4, tmp.end());
					s = "PONG" + s;
					std::cout << "SERVER >> " << s << std::endl;
					s.push_back('\n');
					send(new_socket, s.c_str(), s.size(), 0);
				}
			}
		}

	} 


    // closing the connected socket
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

