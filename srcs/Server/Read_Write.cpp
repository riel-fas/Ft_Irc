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



void    Server::handleWrite(int fd)
{
    Client *client = clients[fd];
    if (!client || client->sendQueue.empty())
        return;

    ssize_t sent = send(fd, client->sendQueue.c_str(), client->sendQueue.size(), 0);

    if (sent < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;     // kernel buffer full — try again next POLLOUT
        disconnectClient(fd);
        return;
    }

    //remove the bytes that were actually sent
    client->sendQueue.erase(0, sent);

    //if queue is now empty, remove POLLOUT flag
    //without this, poll() would wake up constantly (100% CPU spin)
    //so its needed to disable write events when we have nothing to send
    if (client->sendQueue.empty())
        disableWrite(fd);
}
//testing
