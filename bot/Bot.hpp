#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

class Bot {
private:
    std::string _serverIP;
    int         _port;
    std::string _password;
    int         _socketFd;
    std::string _nickname;
    
    std::vector<std::string> _advices;

    void    _initAdvices();
    int     _connectToServer();
    void    _authenticate();
    void    _processMessage(const std::string& message);
    void    _sendMessage(const std::string& message);
    void    _handleSalam(const std::string& target);
    void    _handleNasi7a(const std::string& target);

public:
    Bot(const std::string& ip, int port, const std::string& pass);
    ~Bot();

    void    run();
};

#endif