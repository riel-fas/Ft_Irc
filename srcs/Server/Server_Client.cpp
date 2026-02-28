#include "../../includes/Server.hpp"
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

    std::cout << "New connection from " << client->hostname << " (fd=" << newFd << ")" << std::endl;
}


//old_version
//still in testing
// void    Server::processBuffer(Client &client)
// {
//     std::string &buf  = client.recvbuff;
//     while (true)
//     {
//         std::string::size_type pos = buf.find("\r\n");
//         if (pos == std::string::npos)
//             break; // No complete line yet

//         std::string line = buf.substr(0, pos);
//         buf = buf.substr(pos + 2); // Remove processed line from buffer
//         if(!line.empty())
//             processLine(client, line);
//             //to be donex
//     }
// }


//handles \r and \n
void Server::processBuffer(Client &client)
{
    std::string &buf = client.recvbuff;
    while (true)
    {
        std::string::size_type pos = buf.find("\r\n");
        if (pos == std::string::npos)
            pos = buf.find("\n");  // Also accept just \n
            
        if (pos == std::string::npos)
            break;

        std::string line = buf.substr(0, pos);
        // Remove the delimiter(s)
        if (buf[pos] == '\r' && pos + 1 < buf.size() && buf[pos + 1] == '\n')
            buf = buf.substr(pos + 2);
        else
            buf = buf.substr(pos + 1);
            
        if (!line.empty())
            processLine(client, line);
    }
}


void Server::processLine(Client &client, const std::string &line)
{
    Message msg = parseMessage(line);
    if (msg.command.empty())
        return;

    if      (msg.command == "PASS") handlePass(client, msg);
    else if (msg.command == "NICK") handleNick(client, msg);
    else if (msg.command == "USER") handleUser(client, msg);
    else if (msg.command == "PING") handlePing(client, msg);
    else if (msg.command == "QUIT") disconnectClient(client.fd);
    else if (!client.registered)
        sendToClient(client.fd, makeReply(451, "*", "You have not registered") );
    // ZBEN_OMA!! add JOIN, PRIVMSG, MODE etc here
    // ZBEN_OMA!! plug command handlers here
}


void Server::disconnectClient(int fd)
{
    std::map<int, Client *>::iterator it = clients.find(fd);
    if (it == clients.end())
        return;  // already cleaned up

    std::cout << "Client disconnected (fd=" << fd << ", nick="
              << it->second->nick << ")" << std::endl;

    //YABENMAN remove nick from nickMap
    //ZBEN_OMA broadcast QUIT to all channels the client was in

    // Remove from _fds vector
    std::vector<struct pollfd>::iterator pIt = findPollfd(fd);
    if (pIt != _fds.end())
        _fds.erase(pIt);
    // Close the socket and free memory
    close(fd);
    delete it->second;
    clients.erase(it);
}

void Server::sendToClient(int fd, const std::string &msg)
{
    std::map<int, Client *>::iterator it = clients.find(fd);
    if (it == clients.end())
        return;  //client already disconnected

    it->second->sendQueue += msg;
    //signal poll() that we have data to write
    enableWrite(fd);
}
