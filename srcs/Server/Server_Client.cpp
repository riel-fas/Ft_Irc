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

//handle read event on client 
// Key insight: TCP is a STREAM, not a message protocol.
// recv() may return:
// >>> full IRC message  "NICK alice\r\n"
// >>> partial message   "NICK al" must buffer and wait
// >>> multiple messages   "NICK a\r\nUSER ...\r\n" must handle all
// >>> 0  client disconnected cleanly
// >>> -1 with EAGAIN  no data yet (non-blocking), not an error
void Server::handleRead(int fd)
{
    char buff[512];
    ssize_t bytes  = recv(fd, buff, sizeof(buff) - 1 , 0);
    if(bytes == 0)
    {
        //a disconnect method will be implemented later
        disconnectClient(fd);
        return ;
    }

    if(bytes < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;     // Non-blocking: no data available right now so its not an error
        std::cerr << "recv() error on fd " << fd << ": " << strerror(errno) << std::endl;
        disconnectClient(fd);
        return;
    }

    buff[bytes] = '\0';
    clients[fd]->recvbuff.append(buff, bytes);

    // Now scan the buffer for complete lines
    processBuffer(*clients[fd]);
}


void    Server::processBuffer(Client &client)
{
    std::string &buf  = client.recvbuff;
    while (true)
    {
        std::string::size_type pos = buf.find("\r\n");
        if (pos == std::string::npos)
            break; // No complete line yet

        std::string line = buf.substr(0, pos);
        buf = buf.substr(pos + 2); // Remove processed line from buffer
        if(!line.empty())
            processLine(client, line);
            //to be donex
    }
}


void Server::processLine(Client &client, const std::string &line)
{
    std::cout << "[fd=" << client.fd << "] Received: " << line << std::endl;

    // YABENMAN !!! >>> replaces this with the real IRC message parser
    // ZBEN_OMA !!! >>> replaces the dispatch with the command handlers

    //sendToClient(client.fd, "ECHO: " + line + "\r\n");
    (void)client;
}
