NAME = l7aj_bot
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main_bot.cpp Bot.cpp
HDRS = Bot.hpp
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
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "$(CYAN)  _    _    _    _    _    _    _    _    _    _    _    _    _    _    _    _  $(RESET)"
	@echo "$(CYAN){\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}$(RESET)"
	@echo "$(CYAN) /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\ $(RESET)"
	@echo "$(CYAN)  _                                                                          _  $(RESET)"
	@echo "$(CYAN){\\o/}  $(GREEN).o8                     .        oooo   ooooooooo               o8o $(CYAN){\\o/}$(RESET)"
	@echo "$(CYAN) /_\\  $(GREEN)\"888                   .o8        \`888  d\"\"\"\"\"\"\"8'               \`\"'  $(CYAN)/_\\ $(RESET)"
	@echo "$(CYAN)  _   $(GREEN) 888oooo.   .ooooo.  .o888oo       888        .8'   .oooo.      oooo   $(CYAN)_  $(RESET)"
	@echo "$(CYAN){\\o/}  $(GREEN)d88' \`88b d88' \`88b   888         888       .8'   \`P  )88b     \`888 $(CYAN){\\o/}$(RESET)"
	@echo "$(CYAN) /_\\  $(GREEN) 888   888 888   888   888         888      .8'     .oP\"888      888  $(CYAN)/_\\ $(RESET)"
	@echo "$(CYAN)  _   $(GREEN) 888   888 888   888   888 .       888     .8'     d8(  888      888   $(CYAN)_  $(RESET)"
	@echo "$(CYAN){\\o/}  $(GREEN)\`Y8bod8P' \`Y8bod8P'   \"888\"      o888o   .8'      \`Y888\"\"8o     888 $(CYAN){\\o/}$(RESET)"
	@echo "$(CYAN) /_\\                                                                   888  $(CYAN)/_\\ $(RESET)"
	@echo "$(CYAN)  _                                                                .o. 88P   $(CYAN)_  $(RESET)"
	@echo "$(CYAN){\\o/}                                                              \`Y888P  $(CYAN){\\o/}$(RESET)"
	@echo "$(CYAN) /_\\                                                                        $(CYAN)/_\\ $(RESET)"
	@echo "$(CYAN)  _    _    _    _    _    _    _    _    _    _    _    _    _    _    _    _  $(RESET)"
	@echo "$(CYAN){\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}{\\o/}$(RESET)"
	@echo "$(CYAN) /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\  /_\\ $(RESET)"
	@echo ""
	@echo "$(BOLD)$(PURPLE)╔══════════════════════════════════════════════╗$(RESET)"
	@echo "$(BOLD)$(PURPLE)║             HOW TO USE l7aj_bot              ║$(RESET)"
	@echo "$(BOLD)$(PURPLE)╚══════════════════════════════════════════════╝$(RESET)"
	@echo ""
	@echo "  $(CYAN)Usage:$(RESET)    $(BOLD)./$(NAME) <server_ip> <port> <password>$(RESET)"
	@echo ""
	@echo "  $(CYAN)Example:$(RESET)  $(YELLOW)./$(NAME) 127.0.0.1 6667 mypassword$(RESET)"
	@echo ""
	@echo "$(BOLD)$(PURPLE)╔══════════════════════════════════════════════╗$(RESET)"
	@echo "$(BOLD)$(PURPLE)║                 BOT COMMANDS                 ║$(RESET)"
	@echo "$(BOLD)$(PURPLE)╚══════════════════════════════════════════════╝$(RESET)"
	@echo "  $(CYAN)!salam$(RESET)    Sends a Moroccan greeting"
	@echo "  $(CYAN)!nasi7a$(RESET)   Gives a random Moroccan advice/proverb"
	@echo ""
	@echo "  $(CYAN)Note:$(RESET) You can message the bot in your IRC client using:"
	@echo "        /msg l7aj !nasi7a"
	@echo ""

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re