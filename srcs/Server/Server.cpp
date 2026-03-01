#include "../../includes/Server.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


extern bool g_running;

Server::Server(int port, const std::string &password)
    : _port(port), _serverFd(-1), _password(password)
{
    setupSocket();
}

Server::~Server()
{
    // Close all client connections
    std::map<int, Client *>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it)
    {
        close(it->first);
        delete it->second;
    }
    clients.clear();

    // Close the listening socket
    if (_serverFd != -1)
        close(_serverFd);
}

//--5 steps to setup a server socket
//1. create socket
//2. bind it to an address and port
//3. listen for incoming connections
//4. accept connections
//5. handle client communication(using fcntl >> only for macos)
void Server::setupSocket()
{
    //socket creates an endpoint for communication  it return an int fd
    //the AF_NET is for ipv4 //type of socket //protocol (0 is the default one)
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if(_serverFd < 0)
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));        
    //it get and set options in the socket
    int opt  = 1;
    if(setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error(std::string("setsockopt() failed: ") + strerror(errno));
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; //listen on all interfaces
    server_addr.sin_port        = htons(_port);//convert port to network byte order
    //bind a name to a socket
    if(bind(_serverFd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
    if(listen(_serverFd, SOMAXCONN) < 0)
        throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
    if(fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error(std::string("fcntl() failed: ") + strerror(errno));
    struct pollfd pfd;
    pfd.fd      = _serverFd ;
    pfd.events  = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);
    std::cout <<"Server is listening on port " << _port << std::endl;
}

//the main poll loop
//This function NEVER returns until g_running is set to false by the
//signal handler (Ctrl+C)
//{recv() and send() ----MUST ONLY--- be called from inside this loop}
//in the loop --\/
//option 1 >> acceptClient adds a new entry to _fds — we'll see it
//option 2 >> new connection
//option 3 >> existing client = data arrived
//option 4 >> existing client: ready to send buffered data
//option 5 >> client disconnected or socket error
void Server::run()
{
    while (g_running)
    {
        int num = poll(_fds.data(), _fds.size(), -1);
        if(num < 0)
        {
            if(errno == EINTR)
                continue; //interrupted by signal so retry
            else
                throw std::runtime_error(std::string("poll() failed: ") + strerror(errno));
        }
        for (size_t x = 0; x < _fds.size(); ++x)
        {
            int  fd      = _fds[x].fd;
            int  revents = _fds[x].revents;

            if (revents == 0)
                continue;   // nothing happened on this fd
            if (fd == _serverFd && (revents & POLLIN))
            {
                acceptClient();
                continue;
            }
            if (revents & POLLIN)
                handleRead(fd);
            if (revents & POLLOUT)
                handleWrite(fd);

            if (revents & (POLLHUP | POLLERR | POLLNVAL))
            {
                disconnectClient(fd);
                --x;
                //disconnectClient removes the entry from _fds and erases
                //from clients map >> decrement i so we don't skip the next fd
            }
        }
    }
}