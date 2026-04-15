#include "Bot.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv) 
{
    if (argc != 4) 
    {
        std::cerr << "Usage: ./l7aj_bot <server_ip> <port> <password>\n";
        return 1;
    }
    std::string ip = argv[1];
    int port = atoi(argv[2]);
    std::string pass = argv[3];
    Bot b(ip, port, pass);
    b.run();
    return 0;
}