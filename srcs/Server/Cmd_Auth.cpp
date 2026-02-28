#include "../../includes/Server.hpp"
#include <sstream>
#include <iostream>

//builds a proper IRC numeric reply
//format: :ircserv 001 nick :message\r\n
std::string Server::makeReply(int code, const std::string &target, const std::string &msg)
{
    std::ostringstream ss;
    ss << ":ircserv ";
    if (code < 100) ss << "0";
    if (code < 10)  ss << "0";
    ss << code << " " << target << " :" << msg << "\r\n";
    return ss.str();
}

//PASS <password>
void Server::handlePass(Client &client, const Message &msg)
{
    if (client.registered)
    {
        sendToClient(client.fd, makeReply(462, client.nick, "You may not reregister"));
        return;
    }
    if (msg.params.empty())
    {
        sendToClient(client.fd, makeReply(461, client.nick, "PASS :Not enough parameters"));
        return;
    }
    if (msg.params[0] != _password)
    {
        sendToClient(client.fd, makeReply(464, client.nick, "Password incorrect"));
        disconnectClient(client.fd);
        return;
    }
    client.passOk = true;
}

//NICK <nickname>
void Server::handleNick(Client &client, const Message &msg)
{
    if (msg.params.empty())
    {
        sendToClient(client.fd, makeReply(431, client.nick, "No nickname given"));
        return;
    }
    std::string newNick = msg.params[0];

    // validate — letters, digits, and -_[]{}|\ only, max 9 chars
    if (newNick.size() > 9)
    {
        sendToClient(client.fd, makeReply(432, newNick, "Erroneous nickname"));
        return;
    }
    for (size_t i = 0; i < newNick.size(); i++)
    {
        char c = newNick[i];
        if (!isalnum(c) && c != '-' && c != '_' && c != '[' &&
            c != ']' && c != '{' && c != '}' && c != '|' && c != '\\')
        {
            sendToClient(client.fd, makeReply(432, newNick, "Erroneous nickname"));
            return;
        }
    }
    //YABENMAN!!! check nickMap for duplicates here
    //if (nickMap.count(toLower(newNick))) { send 433; return; }
    client.nick  = newNick;
    client.nickOk = true;

    // if already registered — nick change broadcast
    // YABENMAN!! broadcast :old!u@h NICK newnick to shared channels
    // check if registration now complete
    if (client.passOk && client.nickOk && client.userOk)
    {
        client.registered = true;
        sendWelcome(client);
    }
}

//USER <username> <hostname> <servername> :<realname>
void Server::handleUser(Client &client, const Message &msg)
{
    if (client.registered)
    {
        sendToClient(client.fd, makeReply(462, client.nick, "You may not reregister"));
        return;
    }
    if (msg.params.size() < 4)
    {
        sendToClient(client.fd, makeReply(461, client.nick, "USER :Not enough parameters"));
        return;
    }
    client.user     = msg.params[0];
    client.realname = msg.params[3];
    client.userOk   = true;
    // check if registration now complete
    if (client.passOk && client.nickOk && client.userOk)
    {
        client.registered = true;
        sendWelcome(client);
    }
}

//PING <token> = must reply immediately or client disconnects
void Server::handlePing(Client &client, const Message &msg)
{
    std::string token = msg.params.empty() ? "ircserv" : msg.params[0];
    sendToClient(client.fd, ":ircserv PONG ircserv :" + token + "\r\n");
}

//send 001-004 welcome sequence — LimeChat won't connect without this
void Server::sendWelcome(Client &client)
{
    std::string nick = client.nick;
    std::string mask = nick + "!" + client.user + "@" + client.hostname;

    sendToClient(client.fd, makeReply(001, nick, "Welcome to the IRC Network " + mask));
    sendToClient(client.fd, makeReply(002, nick, "Your host is ircserv, running version 1.0"));
    sendToClient(client.fd, makeReply(003, nick, "This server was created today"));
    sendToClient(client.fd, makeReply(004, nick, "ircserv 1.0 o o"));

    std::cout << "Client registered: " << mask << std::endl;
}