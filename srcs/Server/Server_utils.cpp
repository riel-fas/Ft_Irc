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
#include <cctype>

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


void Server::broadcastToChannel(Channel *chan, const std::string &msg, int excludeFd)
{
    const std::set<int> &members = chan->getMemberFds();
    for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        if (*it != excludeFd)
            sendToClient(*it, msg);
    }
}


std::string Server::toLower(const std::string &str)
{
    std::string lower = str;
    for (size_t i = 0; i < lower.size(); i++)
        lower[i] = std::tolower(static_cast<unsigned char>(lower[i]));
    return lower;
}


std::string Server::itostr(int n)
{
    std::ostringstream ss;
    ss << n;
    return ss.str();
}


std::string Server::trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}


std::string Server::join(const std::vector<std::string> &params, const std::string &delim)
{
    std::string result = "";
    for (size_t i = 0; i < params.size(); ++i) {
        result += params[i];
        if (i < params.size() - 1) result += delim;
    }
    return result;
}
