#include "../includes/Channel.hpp"


Channel::Channel(const std::string &name)
    : _name(name), _topic(""), _key(""), _limit(0),
      _inviteOnly(false), _topicRestricted(true)
{
}


Channel::~Channel()
{
}


// Getters
const std::string &Channel::getName() const { return _name; }
const std::string &Channel::getTopic() const { return _topic; }
const std::string &Channel::getKey() const { return _key; }
int Channel::getLimit() const { return _limit; }
bool Channel::isInviteOnly() const { return _inviteOnly; }
bool Channel::isTopicRestricted() const { return _topicRestricted; }


// Setters
void Channel::setTopic(const std::string &topic) { _topic = topic; }
void Channel::setKey(const std::string &key) { _key = key; }
void Channel::setLimit(int limit) { _limit = limit; }
void Channel::setInviteOnly(bool inviteOnly) { _inviteOnly = inviteOnly; }
void Channel::setTopicRestricted(bool topicRestricted) { _topicRestricted = topicRestricted; }


// Membership management
void Channel::addMember(int fd)
{
    _memberFds.insert(fd);
}


void Channel::removeMember(int fd)
{
    _memberFds.erase(fd);
    _operatorFds.erase(fd);
    _inviteList.erase(fd);
}


bool Channel::isMember(int fd) const
{
    return _memberFds.find(fd) != _memberFds.end();
}


// Privilege management
void Channel::addOperator(int fd)
{
    _operatorFds.insert(fd);
}


void Channel::removeOperator(int fd)
{
    _operatorFds.erase(fd);
}


bool Channel::isOp(int fd) const
{
    return _operatorFds.find(fd) != _operatorFds.end();
}


// Invite management
void Channel::addInvite(int fd)
{
    _inviteList.insert(fd);
}


void Channel::removeInvite(int fd)
{
    _inviteList.erase(fd);
}


bool Channel::isInvited(int fd) const
{
    return _inviteList.find(fd) != _inviteList.end();
}


const std::set<int> &Channel::getMemberFds() const
{
    return _memberFds;
}


size_t Channel::getMemberCount() const
{
    return _memberFds.size();
}



