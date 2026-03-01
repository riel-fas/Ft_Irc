NAME = ircserv

C = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

SRCS =	srcs/main.cpp \
		srcs/Client.cpp \
		srcs/Server/Server.cpp \
		srcs/Server/Server_utils.cpp \
		srcs/Server/Server_Client.cpp \
		srcs/Server/Read_Write.cpp \
		srcs/Server/Parser.cpp \
		srcs/Server/Cmd_Auth.cpp 

HEADER = Client.cpp Server.cpp

OBJS = $(SRCS:.cpp=.o)

# Colors
GREEN = \033[0;32m
CYAN = \033[0;36m
YELLOW = \033[1;33m
RED = \033[0;31m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(GREEN):::::::::: $(CYAN):::::::::::           $(YELLOW)::::::::::: :::::::::   :::::::: $(RESET)"
	@echo "$(GREEN):+:            :+:                   $(CYAN):+:     $(YELLOW):+:    :+: :+:    :+:$(RESET)"
	@echo "$(GREEN)+:+            +:+                   $(CYAN)+:+     $(YELLOW)+:+    +:+ +:+       $(RESET)"
	@echo "$(GREEN):#::+::#       +#+                   $(CYAN)+#+     $(YELLOW)+#++:++#:  +#+       $(RESET)"
	@echo "$(GREEN)+#+            +#+                   $(CYAN)+#+     $(YELLOW)+#+    +#+ +#+       $(RESET)"
	@echo "$(GREEN)#+#            #+#                   $(CYAN)#+#     $(YELLOW)#+#    #+# #+#    #+#$(RESET)"
	@echo "$(GREEN)###            ###    ########## $(CYAN)########### $(YELLOW)###    ###  ######## $(RESET)"
	@echo "$(RESET)"
	$(C) $(FLAGS) -o $(NAME) $(OBJS)
	@echo "$(GREEN)Compilation completed!$(RESET)"

%.o : %.cpp $(HEADER)
	@echo "$(CYAN)Compiling $<...$(RESET)"
	$(C) $(FLAGS) -c $< -o $@ 

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	rm -f $(OBJS)

fclean: clean
	@echo "$(RED)Removing executable...$(RESET)"
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re


#i will add later to the makefile a tldr or a tuto on how to run and use this project so the user wont have problems testing it