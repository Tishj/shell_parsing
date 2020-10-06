# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: rlucas <marvin@codam.nl>                     +#+                      #
#                                                    +#+                       #
#    Created: 2020/04/12 11:11:07 by rlucas        #+#    #+#                  #
#    Updated: 2020/10/06 13:27:44 by tbruinem      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = tokenizer

LIBDIR = lib/libext
SRCDIR = src
OBJDIR = obj
INCLUDES = -I incl -I $(LIBDIR)/incl/

SRC =	main.c

OBJ := $(addprefix $(OBJDIR)/, $(SRC:%.c=%.o))
SRC := $(addprefix $(SRCDIR)/, $(SRC))

FLAGS = 

ifeq ($(DEBUG),1)
	FLAGS += -g -fsanitize=address
endif

all: $(NAME)

$(NAME): $(OBJ)
	@echo "Compiling shell..."
	@gcc $(FLAGS) $(OBJ) -o $(NAME) $(INCLUDES) -L $(LIBDIR)/ -lext

obj/%.o: src/%.c
	@mkdir -p $(@D)
	@echo "Compiling $@"
	gcc -c $(INCLUDES) $(FLAGS) $< -o $@

clean:
	@echo "Removing objects directory..."
	@rm -rf $(OBJDIR)

fclean: clean
	@echo "Removing executable..."
	@rm -rf $(NAME)

re: fclean all
