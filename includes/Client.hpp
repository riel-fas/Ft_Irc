#ifndef CLIENT_HPP
#define CLIENT_HPP 

#include <string>

class Client
{
    public:
        int fd;
        std::string recvbuff;
        std::string sendQueue;
        std::string nick;
        std::string user;
        std::string realname;
        std::string hostname;

        bool passOk;
        bool nickOk;
        bool userOk;
        bool registered;

        Client(int fd);
        ~Client();

    private:
        //prevent copying ;) each client have its own fd so copying makes no sense
        Client(const Client &);
        Client &operator=(const Client &);
};

#endif

