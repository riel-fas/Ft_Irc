#include "Server.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Server::Server(int port, const std::string &password)
    : _port(port), _serverFd(-1), _password(password)
{
    setupSocket();
}

Server::~Server()
{
    // loop over clients map
    // close each fd
    // delete each Client*
    // clear the map
    // close _serverFd
}

//--5 steps to setup a server socket--\\
//1. create socket
//2. bind it to an address and port
//3. listen for incoming connections
//4. accept connections
//5. handle client communication(only for macos)

void Server::setupSocket()
{
    //socket creates an endpoint for commu. it return an int fd
    //the AF_NET is for ipv4 //type of socket //protocol (0 is the default one)
    Server::_serverFd = socket(AF_INET, SOCK_STREAM, 0);
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

    //Added to poll watchlist
    struct pollfd pfd;
    pfd.fd      = _serverFd ;
    pfd.events  = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);

    std::cout <<"Server is listening on port " << _port << std::endl;
}