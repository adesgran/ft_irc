NAME = irc

CC = c++
C_FLAGS = #-Wall -Wextra -Werror -std=c++98

CPP_SERVER = server.cpp
CPP_CLIENT = client.cpp
CPP_FILES = 
O_SERVER = $(CPP_SERVER:.cpp=.o)
O_CLIENT = $(CPP_CLIENT:.cpp=.o)
O_FILES = $(CPP_FILES:.cpp=.o)

INCLUDES = -I .

%.o: %.cpp
	@printf "\033[0;33mGenerating ${NAME} objects... %-33.33s\r\033[0m" $@
	@${CC} ${C_FLAGS} ${INCLUDES} -c $< -o $@

all: server client

server: ${O_FILES} ${O_SERVER} 
	@echo "\n\n\033[0;34mCompiling server...\033[0m\n"
	@${CC} ${O_SERVER} ${O_FILES} -o server

client:  ${O_FILES} ${O_CLIENT} 
	@echo "\n\n\033[0;34mCompiling client...\033[0m\n"
	@${CC} ${O_CLIENT} ${O_FILES} -o client

clean:
	@echo "\n\033[0;31mRemoving objects files...\033[0m"
	@rm -f ${O_FILES} ${O_SERVER} ${O_CLIENT}

fclean: clean
	@echo "\n\033[0;31mRemoving server and client executable...\033[0m\n"
	@rm -f server client

re: fclean all
