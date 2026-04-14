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
#include <cctype>

static bool parsePositiveInt(const std::string &str, int &out)
{
    if (str.empty())
        return false;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(str[i])))
            return false;
    }
    std::istringstream ss(str);
    int value = 0;
    ss >> value;
    if (ss.fail() || value <= 0)
        return false;
    out = value;
    return true;
}

// called when POLLIN (for more infos check signals !!!) fires on the server fd
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
    // make new client(MACOS-only)
    if (fcntl(newFd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl() on client fd failed: " << strerror(errno) << std::endl;
        close(newFd);
        return;
    }
    struct pollfd pfd;
    pfd.fd = newFd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);
    // Create Client object and store it
    Client *client = new Client(newFd);
    client->hostname = inet_ntoa(clientAddr.sin_addr);
    clients[newFd] = client;

    std::cout << "New connection from " << client->hostname << " (fd=" << newFd << ")" << std::endl;
}

// handles \r and \n
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
            pos = buf.find("\n"); // Also accept just \n

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

    if(msg.command == "CAP") 
    {
        if (!msg.params.empty() && msg.params[0] == "LS")
            sendToClient(client.fd, ":ircserv CAP * LS :\r\n");
    }
    else if (msg.command == "PASS")
        handlePass(client, msg);
    else if (msg.command == "NICK")
        handleNick(client, msg);
    else if (msg.command == "USER")
        handleUser(client, msg);
    else if (msg.command == "PING")
        handlePing(client, msg);
    else if (msg.command == "JOIN")
        handleJoin(client, msg);
    else if (msg.command == "PRIVMSG")
        handlePrivmsg(client, msg);
    else if (msg.command == "INVITE")
        handleInvite(client, msg);
    else if (msg.command == "MODE")
        handleMode(client, msg);
    else if (msg.command == "TOPIC")
        handleTopic(client, msg);
    else if (msg.command == "KICK")
        handleKick(client, msg);
    else if (msg.command == "HELP")
        handleHelp(client, msg);
    else if (msg.command == "QUIT")
        disconnectClient(client.fd);
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

    std::vector<std::string> channelNames;
    std::stringstream chanStream(msg.params[0]);
    std::string token;
    while (std::getline(chanStream, token, ','))
        channelNames.push_back(trim(token));

    std::vector<std::string> keys;
    if (msg.params.size() > 1)
    {
        std::stringstream keyStream(msg.params[1]);
        while (std::getline(keyStream, token, ','))
            keys.push_back(trim(token));
    }

    for (size_t i = 0; i < channelNames.size(); ++i)
    {
        std::string channelName = channelNames[i];
        std::string channelKey = (i < keys.size()) ? keys[i] : "";
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

        if (chan->isInviteOnly() && !chan->isInvited(client.fd))
        {
            sendToClient(client.fd, makeReply(473, client.nick, channelName + " :Cannot join channel (+i)"));
            continue;
        }
        if (chan->getLimit() > 0 && static_cast<int>(chan->getMemberCount()) >= chan->getLimit())
        {
            sendToClient(client.fd, makeReply(471, client.nick, channelName + " :Cannot join channel (+l)"));
            continue;
        }
        if (!chan->getKey().empty() && chan->getKey() != channelKey)
        {
            sendToClient(client.fd, makeReply(475, client.nick, channelName + " :Cannot join channel (+k)"));
            continue;
        }

        chan->addMember(client.fd);
        chan->removeInvite(client.fd);
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

void Server::handleInvite(Client &client, const Message &msg)
{
    if (!client.registered)
    {
        sendToClient(client.fd, makeReply(451, "*", "You have not registered"));
        return;
    }
    if (msg.params.size() < 2)
    {
        sendToClient(client.fd, makeReply(461, client.nick, "INVITE :Not enough parameters"));
        return;
    }

    const std::string &targetNick = msg.params[0];
    const std::string &channelName = msg.params[1];

    std::map<std::string, Client *>::iterator nit = nickMap.find(toLower(targetNick));
    if (nit == nickMap.end())
    {
        sendToClient(client.fd, makeReply(401, client.nick, targetNick + " :No such nick/channel"));
        return;
    }

    std::map<std::string, Channel *>::iterator cit = channelMap.find(toLower(channelName));
    if (cit == channelMap.end())
    {
        sendToClient(client.fd, makeReply(403, client.nick, channelName + " :No such channel"));
        return;
    }

    Channel *chan = cit->second;
    if (!chan->isMember(client.fd))
    {
        sendToClient(client.fd, makeReply(442, client.nick, channelName + " :You're not on that channel"));
        return;
    }
    if (chan->isInviteOnly() && !chan->isOp(client.fd))
    {
        sendToClient(client.fd, makeReply(482, client.nick, channelName + " :You're not channel operator"));
        return;
    }
    if (chan->isMember(nit->second->fd))
    {
        sendToClient(client.fd, makeReply(443, client.nick, targetNick + " " + channelName + " :is already on channel"));
        return;
    }

    chan->addInvite(nit->second->fd);
    sendToClient(client.fd, makeReply(341, client.nick, targetNick + " " + channelName));

    std::string inviteMsg = ":" + client.nick + "!" + client.user + "@" + client.hostname +
                            " INVITE " + targetNick + " :" + channelName + "\r\n";
    sendToClient(nit->second->fd, inviteMsg);
}

void Server::handleMode(Client &client, const Message &msg)
{
    if (!client.registered)
    {
        sendToClient(client.fd, makeReply(451, "*", "You have not registered"));
        return;
    }
    if (msg.params.empty())
    {
        sendToClient(client.fd, makeReply(461, client.nick, "MODE :Not enough parameters"));
        return;
    }

    const std::string &target = msg.params[0];
    if (target.empty() || target[0] != '#')
    {
        sendToClient(client.fd, makeReply(501, client.nick, "Unknown MODE flag"));
        return;
    }

    std::map<std::string, Channel *>::iterator it = channelMap.find(toLower(target));
    if (it == channelMap.end())
    {
        sendToClient(client.fd, makeReply(403, client.nick, target + " :No such channel"));
        return;
    }

    Channel *chan = it->second;
    if (msg.params.size() == 1)
    {
        std::string modes = "+";
        std::string modeArgs;
        if (chan->isInviteOnly())
            modes += "i";
        if (chan->isTopicRestricted())
            modes += "t";
        if (!chan->getKey().empty())
            modes += "k";
        if (chan->getLimit() > 0)
            modes += "l";
        if (chan->getLimit() > 0)
            modeArgs = " " + itostr(chan->getLimit());
        sendToClient(client.fd, makeReply(324, client.nick, chan->getName() + " " + modes + modeArgs));
        return;
    }

    if (!chan->isMember(client.fd))
    {
        sendToClient(client.fd, makeReply(442, client.nick, chan->getName() + " :You're not on that channel"));
        return;
    }
    if (!chan->isOp(client.fd))
    {
        sendToClient(client.fd, makeReply(482, client.nick, chan->getName() + " :You're not channel operator"));
        return;
    }

    const std::string &modeString = msg.params[1];
    bool adding = true;
    size_t argIndex = 2;
    std::string modeOut;
    char currentOutSign = '\0';
    std::vector<std::string> modeOutArgs;

    for (size_t i = 0; i < modeString.size(); ++i)
    {
        char m = modeString[i];
        if (m == '+' || m == '-')
        {
            adding = (m == '+');
            continue;
        }

        if (m == 'i')
            chan->setInviteOnly(adding);
        else if (m == 't')
            chan->setTopicRestricted(adding);
        else if (m == 'k')
        {
            if (adding)
            {
                if (argIndex >= msg.params.size())
                {
                    sendToClient(client.fd, makeReply(461, client.nick, "MODE :Not enough parameters"));
                    return;
                }
                chan->setKey(msg.params[argIndex]);
                modeOutArgs.push_back(msg.params[argIndex]);
                ++argIndex;
            }
            else
                chan->setKey("");
        }
        else if (m == 'l')
        {
            if (adding)
            {
                if (argIndex >= msg.params.size())
                {
                    sendToClient(client.fd, makeReply(461, client.nick, "MODE :Not enough parameters"));
                    return;
                }
                int limit = 0;
                if (!parsePositiveInt(msg.params[argIndex], limit))
                {
                    sendToClient(client.fd, makeReply(461, client.nick, "MODE :Not enough parameters"));
                    return;
                }
                chan->setLimit(limit);
                modeOutArgs.push_back(msg.params[argIndex]);
                ++argIndex;
            }
            else
                chan->setLimit(0);
        }
        else if (m == 'o')
        {
            if (argIndex >= msg.params.size())
            {
                sendToClient(client.fd, makeReply(461, client.nick, "MODE :Not enough parameters"));
                return;
            }

            const std::string &opNick = msg.params[argIndex];
            std::map<std::string, Client *>::iterator nit = nickMap.find(toLower(opNick));
            if (nit == nickMap.end() || !chan->isMember(nit->second->fd))
            {
                sendToClient(client.fd, makeReply(441, client.nick, opNick + " " + chan->getName() + " :They aren't on that channel"));
                ++argIndex;
                continue;
            }

            if (adding)
                chan->addOperator(nit->second->fd);
            else
                chan->removeOperator(nit->second->fd);
            modeOutArgs.push_back(opNick);
            ++argIndex;
        }
        else
        {
            sendToClient(client.fd, makeReply(472, client.nick, std::string(1, m) + " :is unknown mode char to me"));
            continue;
        }

        char sign = adding ? '+' : '-';
        if (currentOutSign != sign)
        {
            modeOut += sign;
            currentOutSign = sign;
        }
        modeOut += m;
    }

    if (!modeOut.empty())
    {
        std::string prefix = ":" + client.nick + "!" + client.user + "@" + client.hostname;
        std::string modeMsg = prefix + " MODE " + chan->getName() + " " + modeOut;
        for (size_t i = 0; i < modeOutArgs.size(); ++i)
            modeMsg += " " + modeOutArgs[i];
        modeMsg += "\r\n";
        broadcastToChannel(chan, modeMsg);
    }
}

void Server::handleTopic(Client &client, const Message &msg)
{
    if (!client.registered)
    {
        sendToClient(client.fd, makeReply(451, "*", "You have not registered"));
        return;
    }
    if (msg.params.empty())
    {
        sendToClient(client.fd, makeReply(461, client.nick, "TOPIC :Not enough parameters"));
        return;
    }

    const std::string &channelName = msg.params[0];
    std::map<std::string, Channel *>::iterator it = channelMap.find(toLower(channelName));
    if (it == channelMap.end())
    {
        sendToClient(client.fd, makeReply(403, client.nick, channelName + " :No such channel"));
        return;
    }

    Channel *chan = it->second;
    if (!chan->isMember(client.fd))
    {
        sendToClient(client.fd, makeReply(442, client.nick, channelName + " :You're not on that channel"));
        return;
    }

    if (msg.params.size() == 1)
    {
        if (chan->getTopic().empty())
            sendToClient(client.fd, makeReply(331, client.nick, chan->getName() + " :No topic is set"));
        else
            sendToClient(client.fd, makeReply(332, client.nick, chan->getName() + " :" + chan->getTopic()));
        return;
    }

    if (chan->isTopicRestricted() && !chan->isOp(client.fd))
    {
        sendToClient(client.fd, makeReply(482, client.nick, chan->getName() + " :You're not channel operator"));
        return;
    }

    const std::string &newTopic = msg.params[1];
    chan->setTopic(newTopic);

    std::string prefix = ":" + client.nick + "!" + client.user + "@" + client.hostname;
    std::string topicMsg = prefix + " TOPIC " + chan->getName() + " :" + newTopic + "\r\n";
    broadcastToChannel(chan, topicMsg);
}

void Server::handleKick(Client &client, const Message &msg)
{
    if (!client.registered)
    {
        sendToClient(client.fd, makeReply(451, "*", "You have not registered"));
        return;
    }
    if (msg.params.size() < 2)
    {
        sendToClient(client.fd, makeReply(461, client.nick, "KICK :Not enough parameters"));
        return;
    }

    const std::string &channelName = msg.params[0];
    const std::string &targetNick = msg.params[1];

    std::map<std::string, Channel *>::iterator cit = channelMap.find(toLower(channelName));
    if (cit == channelMap.end())
    {
        sendToClient(client.fd, makeReply(403, client.nick, channelName + " :No such channel"));
        return;
    }

    Channel *chan = cit->second;
    if (!chan->isMember(client.fd))
    {
        sendToClient(client.fd, makeReply(442, client.nick, channelName + " :You're not on that channel"));
        return;
    }
    if (!chan->isOp(client.fd))
    {
        sendToClient(client.fd, makeReply(482, client.nick, channelName + " :You're not channel operator"));
        return;
    }

    std::map<std::string, Client *>::iterator nit = nickMap.find(toLower(targetNick));
    if (nit == nickMap.end())
    {
        sendToClient(client.fd, makeReply(401, client.nick, targetNick + " :No such nick/channel"));
        return;
    }
    if (!chan->isMember(nit->second->fd))
    {
        sendToClient(client.fd, makeReply(441, client.nick, targetNick + " " + channelName + " :They aren't on that channel"));
        return;
    }

    std::string reason = (msg.params.size() > 2) ? msg.params[2] : client.nick;
    std::string prefix = ":" + client.nick + "!" + client.user + "@" + client.hostname;
    std::string kickMsg = prefix + " KICK " + chan->getName() + " " + nit->second->nick + " :" + reason + "\r\n";
    broadcastToChannel(chan, kickMsg);

    chan->removeMember(nit->second->fd);
    if (chan->getMemberCount() == 0)
    {
        delete chan;
        channelMap.erase(cit);
    }
}

void Server::handleHelp(Client &client, const Message &msg)
{
    (void)msg;
    std::string target = client.nick.empty() ? "*" : client.nick;
    sendToClient(client.fd, ":ircserv NOTICE " + target + " :Commands: PASS NICK USER PING JOIN PRIVMSG INVITE MODE TOPIC KICK QUIT HELP\r\n");
}

void Server::disconnectClient(int fd)
{
    std::map<int, Client *>::iterator it = clients.find(fd);
    if (it == clients.end())
        return; // already cleaned up

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
        return; // client already disconnected

    it->second->sendQueue += msg;
    // signal poll() that we have data to write
    enableWrite(fd);
}

/////////////////////////////////////////////////////////
// old_version of process buffer
// still in testing
//  void    Server::processBuffer(Client &client)
//  {
//      std::string &buf  = client.recvbuff;
//      while (true)
//      {
//          std::string::size_type pos = buf.find("\r\n");
//          if (pos == std::string::npos)
//              break; // No complete line yet
//          std::string line = buf.substr(0, pos);
//          buf = buf.substr(pos + 2); // Remove processed line from buffer
//          if(!line.empty())
//              processLine(client, line);
//              //to be donex
//      }
//  }
///////////////////keep in case of a change////////////////////
