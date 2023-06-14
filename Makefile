NAME		=	ircserv

SRCS		=	src/main.cpp	src/utils.cpp	src/Client.cpp	src/Channel.cpp			\
				src/Context.cpp	src/Context_responses.cpp	src/Context_parsing.cpp		\
				src/Context_commands.cpp	src/Context_chanops.cpp						\
				src/Handler_poll.cpp src/Handler_connection.cpp src/Handler_setup.cpp	\
				src/Bot.cpp

SRCS_DIR	=	src

OBJS		=	$(subst $(SRCS_DIR), $(OBJS_DIR), $(SRCS:.cpp=.o))

OBJS_DIR	=	obj

INC			=	-I.

CC			=	c++ -std=c++98 -Wall -Wextra -Werror -g -Iinc/ -Ofast

all:		$(NAME)

$(OBJS_DIR)/%.o :	$(SRCS_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(NAME):	$(MLX) $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(INC)

clean:
	rm -rf $(OBJS_DIR)

fclean:	clean
	rm -rf $(NAME)

re:	fclean all

e:	all
	@clear
	./$(NAME) 1234

h:	re
	@clear
	./$(NAME) 1234

cl:
	# @clear
	@c++ -std=c++98 -Wall -Wextra -Werror -g -I. src/client.cpp -o client
	./client
.PHONY: all clean fclean re e cl