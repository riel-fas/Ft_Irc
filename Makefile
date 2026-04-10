# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: Gemini <ai@42.fr>                          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/10                          #+#    #+#              #
#    Updated: 2026/04/10                          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = ircserv
CC          = c++
FLAGS       = -Wall -Wextra -Werror -std=c++98 -I includes

# Chemins des sources
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

OBJS        = $(SRCS:.cpp=.o)

# Couleurs pour le terminal
GREEN       = \033[0;32m
CYAN        = \033[0;36m
YELLOW      = \033[1;33m
RED         = \033[0;31m
RESET       = \033[0m

# --- RÈGLES ---

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(GREEN):::::::::: $(CYAN):::::::::::           $(YELLOW)::::::::::: :::::::::   :::::::: $(RESET)"
	@echo "$(GREEN):+:             :+:                   $(CYAN):+:     $(YELLOW):+:    :+: :+:    :+:$(RESET)"
	@echo "$(GREEN)+:+             +:+                   $(CYAN)+:+     $(YELLOW)+:+    +:+ +:+       $(RESET)"
	@echo "$(GREEN):#::+::#       +#+                   $(CYAN)+#+     $(YELLOW)+#++:++#:  +#+       $(RESET)"
	@echo "$(GREEN)+#+             +#+                   $(CYAN)+#+     $(YELLOW)+#+    +#+ +#+       $(RESET)"
	@echo "$(GREEN)#+#             #+#                   $(CYAN)#+#     $(YELLOW)#+#    #+# #+#    #+#$(RESET)"
	@echo "$(GREEN)###             ###    ########## $(CYAN)########### $(YELLOW)###    ###  ######## $(RESET)"
	@echo "$(RESET)"
	$(CC) $(FLAGS) -o $(NAME) $(OBJS)
	@echo "$(GREEN)✓ Compilation terminée avec succès !$(RESET)"

# Règle générique pour les fichiers objets
# Dépend des .cpp ET des .hpp pour recompiler si un header change
%.o: %.cpp $(HEADER)
	@echo "$(CYAN)Compiling $<...$(RESET)"
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	@echo "$(RED)Nettoyage des objets...$(RESET)"
	@rm -f $(OBJS)

fclean: clean
	@echo "$(RED)Suppression de l'exécutable $(NAME)...$(RESET)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re