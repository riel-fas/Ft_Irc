#include <iostream>
#include <csignal>
#include <cstdlib>
#include <cerrno>
#include "../includes/Server.hpp"



bool g_running = true;

static void signal_handler(int sig)
{
    // suppresses <-Wextra> flag  unused parameter warning
    (void)sig;        
    g_running = false;
}

static int validate_port(const char *str)
{
    char *end;
    errno = 0;
    long val = strtol(str, &end, 10);
    //i think a problem will occure if we tested using a known port 
    //0 - 20 - 21 - 22 - 23 - 25 - 53 - 57 
    //- 67/68 - 80 - 110 - 123 - 137/139 
    //- 143 - 161 - 443 - 445 - 631 
    if (errno != 0 || end == str || *end != '\0')
        return -1;
    if (val < 1024 || val > 65535)
        return -1;
    return static_cast<int>(val);
}
//we used strol instead of atoi because 
//it provides better error handling 
//and can detect invalid input more robustly

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    //we need to validate port because max port is 65,535
    int port = validate_port(av[1]);
    if(port == -1)
    {
        std::cerr << "Invalid port number: " << av[1] << std::endl;
        return 1;
    }
    std::string password(av[2]);
    if(password.empty())
    {
        std::cerr << "Password cannot be empty" << std::endl;
        return 1;
    }

    //ctrl/c + ctrl/backslash + broken pipe
    signal(SIGINT,  signal_handler);  
    signal(SIGQUIT, signal_handler);  
    signal(SIGPIPE, SIG_IGN);         


    try
    {
        Server server(port, password);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;

    }
    return 0;

    //and setting the server 
}

//testing
