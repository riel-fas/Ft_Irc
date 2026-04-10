#ifndef SERVER_HPP
#define SERVER_HPP


#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <set>
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"


class Server
{
    public:
        Server(int port, const std::string &password);
       
        void run();
        void sendToClient(int fd, const std::string &msg);
       
        std::map<int, Client *> clients;
        std::map<std::string, Client *> nickMap;
        std::map<std::string, Channel *> channelMap;


        ~Server();


    private:
        Server(const Server &);
        Server &operator=(const Server &);


        int                        _port;
        int                        _serverFd;
        std::vector<struct pollfd> _fds;
        std::string                _password;


        void setupSocket();
        void acceptClient();
        void handleRead(int fd);
        void handleWrite(int fd);
        void disconnectClient(int fd);
        void processBuffer(Client &client);
        void processLine(Client &client, const std::string &line);
        void enableWrite(int fd);
        void disableWrite(int fd);
        std::vector<struct pollfd>::iterator findPollfd(int fd);
        Message parseMessage(const std::string &line);
        // auth commands
        void handlePass(Client &client, const Message &msg);
        void handleNick(Client &client, const Message &msg);
        void handleUser(Client &client, const Message &msg);
        void handlePing(Client &client, const Message &msg);
        void sendWelcome(Client &client);
        // broadcast
        void broadcastToChannel(Channel *chan, const std::string &msg, int excludeFd = -1);
        // utility
        std::string makeReply(int code, const std::string &target, const std::string &msg);
        std::string toLower(const std::string &str);
        std::string itostr(int n);
        std::string trim(const std::string &str);
        std::string join(const std::vector<std::string> &params, const std::string &delim);
};




#endif


