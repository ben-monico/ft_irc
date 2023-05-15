NAME = ircserv

BUILTINS_DIR =	builtins/

SRC_DIR = src/

OBJ_DIR = obj/

SOURCES = 	main.cpp

SRCS = $(addprefix $(SRC_DIR), $(SOURCES))

OBJECTS = $(SOURCES:.c=.o)

OBJS =	$(addprefix $(OBJ_DIR), $(OBJECTS))

CC = c++ -std=c++98 -Wall -Wextra -Werror -Wshadow -g 

$(OBJ_DIR)%.o: $(SRC_DIR)%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) $(DEBUG) $(INC) -c $< -o $@

all:		$(NAME)

$(NAME):	$(OBJS)  $(SRCS)
			@$(CC) $(OBJS) -o $(NAME)
			@clear

$(OBJ_DIR):
			mkdir -p $(OBJ_DIR)$(BUILTINS_DIR)

re: fclean all

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

.PHONY: all re clean fclean