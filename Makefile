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
	@echo "$(CYAN)  _    _    _    _    _    _    _    _    _    _    _    _    _  $(RESET)"
	@echo "$(CYAN){\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}$(RESET)"
	@echo "$(CYAN) /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\ $(RESET)"
	@echo "$(CYAN)  _                                                           _  $(RESET)"
	@echo "$(CYAN){\\o/}  $(GREEN).o88o.     .                o8o                      $(CYAN){\\o/}$(RESET)"
	@echo "$(CYAN) /_\\   $(GREEN)888 \`\"   .o8                \`\"'                       $(CYAN)/_\\ $(RESET)"
	@echo "$(CYAN)  _   $(GREEN)o888oo  .o888oo             oooo  oooo d8b  .ooooo.     $(CYAN)_  $(RESET)"
	@echo "$(CYAN){\\o/}  $(GREEN)888      888               \`888  \`888\"\"8P d88' \`\"Y8  $(CYAN){\\o/}$(RESET)"
	@echo "$(CYAN) /_\\   $(GREEN)888      888                888   888     888         $(CYAN)/_\\ $(RESET)"
	@echo "$(CYAN)  _    $(GREEN)888      888 .              888   888     888   .o8    $(CYAN)_  $(RESET)"
	@echo "$(CYAN){\\o/} $(GREEN)o888o     \"888\" ooooooooooo o888o d888b    \`Y8bod8P'  $(CYAN){\\o/}$(RESET)"
	@echo "$(CYAN) /_\\                                                         /_\\ $(RESET)"
	@echo "$(CYAN)  _    _    _    _    _    _    _    _    _    _    _    _    _  $(RESET)"
	@echo "$(CYAN){\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}$(RESET)"
	@echo "$(CYAN) /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\ $(RESET)"
	@echo "                $(BOLD)$(PURPLE)by : riel-fas, zben-oma, yabenman$(RESET)"
	@echo "$(RESET)"
	$(C) $(FLAGS) -o $(NAME) $(OBJS)
	@echo "$(GREEN)✓ Compilation completed successfully!$(RESET)"
	@echo ""
	@echo "$(BOLD)$(PURPLE)╔══════════════════════════════════════════════╗$(RESET)"
	@echo "$(BOLD)$(PURPLE)║           HOW TO RUN ft_irc                  ║$(RESET)"
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
	@echo "$(BOLD)$(PURPLE)╔══════════════════════════════════════════════╗$(RESET)"
	@echo "$(BOLD)$(PURPLE)║           SUPPORTED COMMANDS & MODES         ║$(RESET)"
	@echo "$(BOLD)$(PURPLE)╚══════════════════════════════════════════════╝$(RESET)"
	@echo "  $(CYAN)PASS$(RESET)     <password>"
	@echo "  $(CYAN)NICK$(RESET)     <nickname>"
	@echo "  $(CYAN)USER$(RESET)     <username> 0 * :<realname>"
	@echo "  $(CYAN)JOIN$(RESET)     <#channel> [key]"
	@echo "  $(CYAN)PRIVMSG$(RESET)  <target> :<message>"
	@echo "  $(CYAN)MODE$(RESET)     <#channel> <+/- i|t|k|o|l> [args]"
	@echo "             $(YELLOW)i$(RESET): Invite-only channel"
	@echo "             $(YELLOW)t$(RESET): Topic restricted to operators"
	@echo "             $(YELLOW)k$(RESET): Set/remove channel key (password)"
	@echo "             $(YELLOW)o$(RESET): Give/take operator privilege"
	@echo "             $(YELLOW)l$(RESET): Set/remove user limit"
	@echo "  $(CYAN)TOPIC$(RESET)    <#channel> [:<new_topic>]"
	@echo "  $(CYAN)INVITE$(RESET)   <nickname> <#channel>"
	@echo "  $(CYAN)KICK$(RESET)     <#channel> <nickname> [:<reason>]"
	@echo "  $(CYAN)QUIT$(RESET)     [:<reason>]"
	@echo ""

bonus:
	@echo "$(CYAN)Compiling l7aj_bot (bonus)...$(RESET)"
	$(MAKE) -C bot
	@echo "$(GREEN)✓ l7aj_bot built successfully!$(RESET)"

%.o : %.cpp $(HEADER)
	@echo "$(CYAN)Compiling $<...$(RESET)"
	$(C) $(FLAGS) -c $< -o $@ 

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	rm -f $(OBJS)
	$(MAKE) -C bot clean

fclean: clean
	@echo "$(RED)Removing executable...$(RESET)"
	rm -f $(NAME)
	$(MAKE) -C bot fclean

re: fclean all

.PHONY: all clean fclean re bonus