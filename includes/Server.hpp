#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include "Client.hpp"
#include "Message.hpp"

class Server
{
    public:
        Server(int port, const std::string &password);
        
        void run();
        void sendToClient(int fd, const std::string &msg);
        std::map<int, Client *> clients;

        ~Server();

    private:
        Server(const Server &);
        Server &operator=(const Server &);

        int                        _port;
        int                        _serverFd;
        std::vector<struct pollfd> _fds;
        std::string                _password;

        void setupSocket();//done
        void acceptClient();//done
        void handleRead(int fd);
        void handleWrite(int fd);
        void disconnectClient(int fd);
        void processBuffer(Client &client);
        void processLine(Client &client, const std::string &line);
        void enableWrite(int fd);
        void disableWrite(int fd);
        std::vector<struct pollfd>::iterator findPollfd(int fd);

        Message Server::parseMessage(const std::string &line);
};

#endif

//testing
