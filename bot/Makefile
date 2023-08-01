NAME = bot

CC = c++
CXX_FLAGS = -Wall -Wextra -Werror
CXX_FLAGS += -std=c++98
CPP_FLAGS = -MMD

CXX_SRCS = main Bot utils
CXX_FILES = $(addsuffix .cpp, $(addprefix srcs/, ${CXX_SRCS}))

O_FILES = $(CXX_FILES:.cpp=.o)
D_FILES = $(O_FILES:.o=.d)

INCLUDES = -I includes

all: ${NAME}

-include : $(D_FILES)

%.o: %.cpp
	@printf "\033[0;33mGenerating ${NAME} objects... %-33.33s\r\033[0m" $@
	@${CC} ${CXX_FLAGS} ${CPP_FLAGS} ${INCLUDES} -c $< -o $@

${NAME}: ${O_FILES} 
	@mkdir -p log
	@echo "\n\n\033[0;34mCompiling ${NAME}...\033[0m\n"
	@${CC} ${O_FILES} -o ${NAME}

clean:
	@echo "\n\033[0;31mRemoving objects files...\033[0m"
	@rm -f ${O_FILES} ${D_FILES}

fclean: clean
	@echo "\n\033[0;31mRemoving ${NAME} executable...\033[0m\n"
	@rm -f ${NAME}

re: fclean all
