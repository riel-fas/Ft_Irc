NAME = ircserv

C = c++
FLAGS = -Wall -Wextra -Werror -std=c++98

SRCS        = srcs/main.cpp \
              srcs/Client.cpp \
              srcs/Channel.cpp \
              srcs/Server/Server.cpp \
              srcs/Server/Server_utils.cpp \
              srcs/Server/Server_Client.cpp \
              srcs/Server/Read_Write.cpp \
              srcs/Server/Parser.cpp \
              srcs/Server/Cmd_Auth.cpp

HEADER      = includes/Client.hpp \
              includes/Server.hpp \
              includes/Message.hpp \
              includes/Channel.hpp


OBJS = $(SRCS:.cpp=.o)

# Colors
GREEN  = \033[0;32m
CYAN   = \033[0;36m
YELLOW = \033[1;33m
RED    = \033[0;31m
BOLD   = \033[1m
PURPLE = \033[0;35m
RESET  = \033[0m

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
	@echo "$(GREEN)✓ Compilation completed successfully!$(RESET)"
	@echo ""
	@echo "$(BOLD)$(PURPLE)╔══════════════════════════════════════════════╗$(RESET)"
	@echo "$(BOLD)$(PURPLE)║           HOW TO RUN ft_irc                 ║$(RESET)"
	@echo "$(BOLD)$(PURPLE)╚══════════════════════════════════════════════╝$(RESET)"
	@echo ""
	@echo "  $(CYAN)Usage:$(RESET)    $(BOLD)./$(NAME) <port> <password>$(RESET)"
	@echo ""
	@echo "  $(CYAN)Example:$(RESET)  $(YELLOW)./$(NAME) 6667 mypassword$(RESET)"
	@echo ""
	@echo "  $(CYAN)Connect:$(RESET)  HexChat → 127.0.0.1 / port 6667 / password"
	@echo "            nc -C 127.0.0.1 6667"
	@echo ""
	@echo "  $(CYAN)Port:$(RESET)     Must be between 1024 and 65535"
	@echo "  $(CYAN)Password:$(RESET) Cannot be empty"
	@echo ""

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

