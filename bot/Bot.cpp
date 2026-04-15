#include "Bot.hpp"

//the bot will auth like any other user , it will be working with 2 cmds SALAM && NASI7A

Bot::Bot(const std::string& ip, int port, const std::string& pass)
    : _serverIP(ip), _port(port), _password(pass), _socketFd(-1), _nickname("l7aj") 
    {
    _initAdvices();
}

Bot::~Bot() {
    if (_socketFd != -1) 
        close(_socketFd);
}

void Bot::_initAdvices() 
{
    _advices.push_back("Li bgha l3ssel ysbar l9ris n7al!");
    _advices.push_back("Dir lkhayr wa nsah, dir charr wa tfakrou!");
    _advices.push_back("7ta mcha l3rs 3ad ja l3ma ychta7!");
    _advices.push_back("Jdid lih jda welbali la tfarrat fih!");
    _advices.push_back("Li ma 3erfakch ma ykadrekch!");
    _advices.push_back("Zwwe9 tbiiii3!");
    _advices.push_back("Dirha zzwina tel9aha zzwina!");
    _advices.push_back("7ekmat lwe9t: la tezreb 3la rez9ek!");
    _advices.push_back("Sidi 3mar b9a f ddar!");
}

int Bot::_connectToServer() 
{
    struct sockaddr_in serverAddr;
    _socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFd < 0) 
    {
        std::cerr << "Bot: Failed to create socket\n";
        return -1;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = inet_addr(_serverIP.c_str());
    if (connect(_socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bot: Failed to connect to server\n";
        return -1;
    }
    std::cout << "Bot: Connected to IRC server at " << _serverIP << ":" << _port << "\n";
    return 0;
}

void Bot::_sendMessage(const std::string& message) 
{
    std::string to_send = message + "\r\n";
    send(_socketFd, to_send.c_str(), to_send.length(), 0);
}

void Bot::_authenticate() 
{
    if (!_password.empty()) 
        _sendMessage("PASS " + _password);
    _sendMessage("NICK " + _nickname);
    _sendMessage("USER " + _nickname + " 0 * :" + _nickname + " wa7ed jwan bot");
}

void Bot::_handleSalam(const std::string& target) 
{
    _sendMessage("PRIVMSG " + target + " :WA ALAYKOUM SALAM! ACH BGHITI!!!!!!!!!");
}

void Bot::_handleNasi7a(const std::string& target) 
{
    if (!_advices.empty()) 
    {
        int index = rand() % _advices.size();
        _sendMessage("PRIVMSG " + target + " :SME3NI N9OLEK: " + _advices[index]);
    }
}

void Bot::_processMessage(const std::string& message) 
{
    //testing the ping to PING from server
    if (message.substr(0, 4) == "PING") 
    {
        std::string pong = "PONG " + message.substr(5);
        _sendMessage(pong);
        return;
    }
    size_t privmsgPos = message.find(" PRIVMSG ");
    if (message[0] == ':' && privmsgPos != std::string::npos) 
    {
        size_t senderEnd = message.find('!', 1);
        if (senderEnd == std::string::npos || senderEnd > privmsgPos) 
            senderEnd = privmsgPos;
        std::string sender = message.substr(1, senderEnd - 1);

        size_t targetStart = privmsgPos + 9;
        size_t targetEnd = message.find(" :", targetStart);
        if (targetEnd != std::string::npos) 
        {
            std::string target = message.substr(targetStart, targetEnd - targetStart);
            std::string text = message.substr(targetEnd + 2);
            std::string replyTarget = (target == _nickname) ? sender : target;
            if (text == "!salam" || text.find("!salam ") == 0) 
                _handleSalam(replyTarget);
            else if (text == "!nasi7a" || text.find("!nasi7a ") == 0) 
                _handleNasi7a(replyTarget);
        }
    }
}

void Bot::run() 
{
        srand(time(NULL));

    if (_connectToServer() < 0)
        return;
    _authenticate();
    char buffer[1024];
    std::string incompleteMessage = "";
    while (true) 
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(_socketFd, buffer, sizeof(buffer) - 1, 0);    
        if (bytesReceived <= 0) 
        {
            std::cout << "Bot: Disconnected from server\n";
            break;
        }
        incompleteMessage += buffer;
        size_t pos;
        while ((pos = incompleteMessage.find("\r\n")) != std::string::npos) 
        {
            std::string message = incompleteMessage.substr(0, pos);
            std::cout << "<- " << message << "\n";
            _processMessage(message);
            incompleteMessage = incompleteMessage.substr(pos + 2);
        }
    }
}