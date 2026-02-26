#include "Server.hpp"

#include <iostream>
#include <sstream>      
#include <stdexcept>    
#include <cstring>      
#include <cerrno>      
#include <unistd.h>    
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>


//called when POLLIN fires on the server fd
void Server::acceptClient()
{
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);

    int newFd = accept(_serverFd, (struct sockaddr *)&clientAddr, &len);
    if (newFd < 0)
    {
        // EAGAIN/EWOULDBLOCK can happen on non-blocking accept — not a real error
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            std::cerr << "accept() error: " << strerror(errno) << std::endl;
        return;
    }
    //make new client(MACOS-only)
    if (fcntl(newFd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl() on client fd failed: " << strerror(errno) << std::endl;
        close(newFd);
        return;
    }

    struct pollfd pfd;
    pfd.fd      = newFd;
    pfd.events  = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);

    // Create Client object and store it
    Client *client   = new Client(newFd);
    client->hostname = inet_ntoa(clientAddr.sin_addr);
    clients[newFd]   = client;

    std::cout << "New connection from " << client->hostname
              << " (fd=" << newFd << ")" << std::endl;
}