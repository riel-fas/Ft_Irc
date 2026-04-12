#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
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


//called when POLLIN (for more infos check signals !!!) fires on the server fd
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




//handles \r and \n
void Server::processBuffer(Client &client)
{
    int clientFd = client.fd;
    while (true)
    {
        // check if client was disconnected in a previous command
        std::map<int, Client *>::iterator it = clients.find(clientFd);
        if (it == clients.end())
            break;

        std::string &buf = client.recvbuff;

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
    else if (msg.command == "JOIN") handleJoin(client, msg);
    else if (msg.command == "PRIVMSG") handlePrivmsg(client, msg);
    else if (msg.command == "QUIT") disconnectClient(client.fd);
    else
    {
        if (!client.registered)
            sendToClient(client.fd, makeReply(451, "*", "You have not registered"));
        else
            sendToClient(client.fd, makeReply(421, client.nick, msg.command + " :Unknown command"));
    }
    // ZBEN_OMA yonaffid hona
}


void Server::handleJoin(Client &client, const Message &msg)
{
    if (!client.registered)
    {
        sendToClient(client.fd, makeReply(451, "*", "You have not registered"));
        return;
    }
    if (msg.params.empty())
    {
        sendToClient(client.fd, makeReply(461, client.nick, "Not enough parameters"));
        return;
    }

    std::stringstream chanStream(msg.params[0]);
    std::string channelName;
    while (std::getline(chanStream, channelName, ','))
    {
        channelName = trim(channelName);
        if (channelName.empty() || channelName[0] != '#')
        {
            sendToClient(client.fd, makeReply(476, client.nick, channelName + " :Invalid channel name"));
            continue;
        }

        Channel *chan;
        std::map<std::string, Channel *>::iterator it = channelMap.find(toLower(channelName));
        if (it == channelMap.end())
        {
            chan = new Channel(channelName);
            channelMap[toLower(channelName)] = chan;
        }
        else
            chan = it->second;

        if (chan->isMember(client.fd))
            continue;

        chan->addMember(client.fd);
        if (chan->getMemberCount() == 1)
            chan->addOperator(client.fd);

        std::string prefix = ":" + client.nick + "!" + client.user + "@" + client.hostname;
        std::string joinMsg = prefix + " JOIN " + chan->getName() + "\r\n";
        broadcastToChannel(chan, joinMsg);

        if (chan->getTopic().empty())
            sendToClient(client.fd, makeReply(331, client.nick, chan->getName() + " :No topic is set"));
        else
            sendToClient(client.fd, makeReply(332, client.nick, chan->getName() + " :" + chan->getTopic()));

        std::string names;
        const std::set<int> &members = chan->getMemberFds();
        for (std::set<int>::const_iterator mit = members.begin(); mit != members.end(); ++mit)
        {
            std::map<int, Client *>::iterator cit = clients.find(*mit);
            if (cit == clients.end())
                continue;
            if (!names.empty())
                names += " ";
            if (chan->isOp(*mit))
                names += "@";
            names += cit->second->nick;
        }
        sendToClient(client.fd, makeReply(353, client.nick, "= " + chan->getName() + " :" + names));
        sendToClient(client.fd, makeReply(366, client.nick, chan->getName() + " :End of /NAMES list"));
    }
}


void Server::handlePrivmsg(Client &client, const Message &msg)
{
    if (!client.registered)
    {
        sendToClient(client.fd, makeReply(451, "*", "You have not registered"));
        return;
    }
    if (msg.params.empty())
    {
        sendToClient(client.fd, makeReply(411, client.nick, "No recipient given (PRIVMSG)"));
        return;
    }
    if (msg.params.size() < 2 || msg.params[1].empty())
    {
        sendToClient(client.fd, makeReply(412, client.nick, "No text to send"));
        return;
    }

    const std::string &target = msg.params[0];
    const std::string &text = msg.params[1];
    std::string wire = ":" + client.nick + "!" + client.user + "@" + client.hostname +
                       " PRIVMSG " + target + " :" + text + "\r\n";

    if (!target.empty() && target[0] == '#')
    {
        std::map<std::string, Channel *>::iterator it = channelMap.find(toLower(target));
        if (it == channelMap.end())
        {
            sendToClient(client.fd, makeReply(401, client.nick, target + " :No such nick/channel"));
            return;
        }
        Channel *chan = it->second;
        if (!chan->isMember(client.fd))
        {
            sendToClient(client.fd, makeReply(404, client.nick, target + " :Cannot send to channel"));
            return;
        }
        broadcastToChannel(chan, wire, client.fd);
        return;
    }

    std::map<std::string, Client *>::iterator nit = nickMap.find(toLower(target));
    if (nit == nickMap.end())
    {
        sendToClient(client.fd, makeReply(401, client.nick, target + " :No such nick/channel"));
        return;
    }
    sendToClient(nit->second->fd, wire);
}


void Server::disconnectClient(int fd)
{
    std::map<int, Client *>::iterator it = clients.find(fd);
    if (it == clients.end())
        return;  // already cleaned up


    std::cout << "Client disconnected (fd=" << fd << ", nick="
              << it->second->nick << ")" << std::endl;


    // Remove from nickMap
    if (!it->second->nick.empty())
        nickMap.erase(toLower(it->second->nick));


    // Remove from all channels
    std::map<std::string, Channel *>::iterator cit = channelMap.begin();
    while (cit != channelMap.end())
    {
        Channel *chan = cit->second;
        if (chan->isMember(fd))
        {
            chan->removeMember(fd);
            // ZBEN_OMA broadcast QUIT to shared channels will be added in Phase 3
            if (chan->getMemberCount() == 0)
            {
                delete chan;
                channelMap.erase(cit++);
                continue;
            }
        }
        ++cit;
    }


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




/////////////////////////////////////////////////////////
//old_version of process buffer
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
///////////////////keep in case of a change////////////////////



