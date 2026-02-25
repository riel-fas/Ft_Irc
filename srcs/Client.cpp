#include "Client.hpp"

#include <iostream>

Client::Client(int fd)
    : fd(fd),
      recvbuff(""),
      sendQueue(""),
      nick("*"),
      user(""),
      realname(""),
      hostname(""),
      passOk(false),
      nickOk(false),
      userOk(false),
      registered(false)
{
}

Client::~Client()
{
    // fd is closed by Server before deleting Client
    // nothing to free here yet
}