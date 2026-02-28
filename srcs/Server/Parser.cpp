#include "../../includes/Server.hpp"
#include "../../includes/Message.hpp"
#include <iostream>

//step 1 extract prefix if line starts with :
//step 2 extract command
//step 3 extract parameters
Message Server::parseMessage(const std::string &line)
{
    Message msg;
    std::string::size_type pos = 0;
    std::string::size_type end = line.size();

    if (pos < end && line[pos] == ':')
    {
        ++pos;
        std::string::size_type prefix_end = line.find(' ', pos);
        if (prefix_end == std::string::npos)
            return msg; //no command found = invalid
        msg.prefix = line.substr(pos, prefix_end - pos);
        pos = prefix_end + 1;
    }

    std::string::size_type command_end = line.find(' ', pos);
    if (command_end == std::string::npos)
        command_end = end;
    msg.command = line.substr(pos, command_end - pos);

    //uppercase the command from  nick to NICK
    for (std::string::size_type i = 0; i < msg.command.size(); i++)
        msg.command[i] = std::toupper((unsigned char)msg.command[i]);
    //no params so were done
    if (command_end >= end)
        return msg;
    pos = command_end + 1;

    while (pos < end)
    {
        if (line[pos] == ':')
        {
            //trailing param = rest of line is ONE parameter[spaces allowed]
            msg.params.push_back(line.substr(pos + 1));
            break;
        }
        std::string::size_type param_end = line.find(' ', pos);
        if (param_end == std::string::npos)
            param_end = end;
        msg.params.push_back(line.substr(pos, param_end - pos));
        if (param_end >= end)
            break;
        pos = param_end + 1;
    }
    return msg;
}