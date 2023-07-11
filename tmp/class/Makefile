NAME = irc

CC = c++
C_FLAGS = -Wall -Wextra -Werror -std=c++98

CPP_FILES = main.cpp User.cpp Server.cpp Channel.cpp message_reader.cpp
O_FILES = $(CPP_FILES:.cpp=.o)

INCLUDES = -I .

%.o: %.cpp
	@printf "\033[0;33mGenerating ${NAME} objects... %-33.33s\r\033[0m" $@
	@${CC} ${C_FLAGS} ${INCLUDES} -c $< -o $@

all: ${NAME}

${NAME}:  ${O_FILES} 
	@echo "\n\n\033[0;34mCompiling ${NAME}...\033[0m\n"
	@${CC} ${O_FILES} -o ${NAME}

valgrind: ${NAME}
	valgrind ./${NAME}

clean:
	@echo "\n\033[0;31mRemoving objects files...\033[0m"
	@rm -f ${O_FILES} 

fclean: clean
	@echo "\n\033[0;31mRemoving ${NAME} executable...\033[0m\n"
	@rm -f ${NAME}

re: fclean all
