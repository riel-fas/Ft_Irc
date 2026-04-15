#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <vector>
class Channel
{
    private:
        std::string _name;
        std::string _topic;
        std::string _key;
        int         _limit;
        bool        _inviteOnly;
        bool        _topicRestricted;

        std::set<int> _memberFds;
        std::set<int> _operatorFds;
        std::set<int> _inviteList;

    public:
        Channel(const std::string &name);
        ~Channel();

        const std::string &getName() const;
        const std::string &getTopic() const;
        const std::string &getKey() const;
        int               getLimit() const;
        bool              isInviteOnly() const;
        bool              isTopicRestricted() const;
        void setTopic(const std::string &topic);
        void setKey(const std::string &key);
        void setLimit(int limit);
        void setInviteOnly(bool inviteOnly);
        void setTopicRestricted(bool topicRestricted);

        //membership management
        void addMember(int fd);
        void removeMember(int fd);
        bool isMember(int fd) const;

        //privilege management
        void addOperator(int fd);
        void removeOperator(int fd);
        bool isOp(int fd) const;

        //invite management
        void addInvite(int fd);
        void removeInvite(int fd);
        bool isInvited(int fd) const;

        //file descriptor management
        const std::set<int> &getMemberFds() const;
        size_t              getMemberCount() const;
};


#endif



