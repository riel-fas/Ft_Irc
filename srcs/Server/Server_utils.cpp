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



std::vector<struct pollfd>::iterator Server::findPollfd(int fd)
{
    std::vector<struct pollfd>::iterator it = _fds.begin();
    for (; it != _fds.end(); ++it)
    {
        if (it->fd == fd)
            return it;
    }
    return _fds.end();
}


void Server::enableWrite(int fd)
{
    std::vector<struct pollfd>::iterator it = findPollfd(fd);
    if (it != _fds.end())
        it->events |= POLLOUT;   //bitwise OR — adds POLLOUT, keeps POLLIN
}


void Server::disableWrite(int fd)
{
    std::vector<struct pollfd>::iterator it = findPollfd(fd);
    if (it != _fds.end())
        it->events &= ~POLLOUT;  //bitwise AND NOT — removes POLLOUT, keeps POLLIN
}