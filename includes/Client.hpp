#ifndef CLIENT_HPP
#define CLIENT_HPP 

#include <string>

class Client
{
    public:
        int fd;
        std::string recvbuff;
        std::string sendQueue;
        //will be needed for {yabenman} for (user idenetity)
        std::string nick;
        std::string user;
        std::string realname;
        std::string hostname;
        //will be needed for authentication (yabenman) for (password)
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
//PS: no need to respect the canonical form no mention to it in the subject