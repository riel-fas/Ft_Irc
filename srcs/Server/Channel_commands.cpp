#include "../../includes/Channel.hpp"

void Channel::handleJoin(Client &client, const Message &msg)
{
    if (msg.params.empty())
    {
        sendToClient(client.fd, makeReply(461, client.nick, "JOIN :Not enough parameters"));
        return;
    }
    std::string channelName = msg.params[0];
    if (channelName[0] != '#')
    {
        sendToClient(client.fd, makeReply(476, client.nick, channelName + " :Invalid channel name"));
        return;
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
}