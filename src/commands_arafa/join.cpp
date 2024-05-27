#include "Server.hpp"

void channelReply(Server *server, const int &fdUser, std::string channelName,
    std::vector<std::string> parameters)
{
    // Reply if the user successfully joined the channel
    // Use send for all the user of a channel (vector of fd)
    std::string event = clientReply(server, fdUser, "JOIN " + channelName);
    std::vector<std::string> parameterTopic;

    parameterTopic.push_back(channelName);
    server->sendChannel(channelName, event);
    topic(fdUser, parameterTopic, channelName, server);
    names(fdUser, parameters, channelName, server);
}

void createChannel(std::string channel, const size_t &pos,
                    std::vector<std::string> key, User *currentUser, Server *server)
{
    Channel *newChannel;
    std::vector<std::string>::iterator it;

    if (key.empty() == false)
    {
        it = key.begin() + pos;
        if (key.size() > pos && (*it).compare("x") != 0)
            newChannel = new Channel(channel, *it, currentUser);
        else
            newChannel = new Channel(channel, currentUser);
        server->_channelList.insert(std::pair<std::string, Channel *>(channel,
                                                                      newChannel));
        currentUser->addChannelJoined(channel);
    }
    else
    {
        newChannel = new Channel(channel, currentUser);
        server->_channelList.insert(std::pair<std::string, Channel *>(channel,
                                                                      newChannel));
        currentUser->addChannelJoined(channel);
    }
}

int checkKey(size_t pos, std::vector<std::string> key,
             std::map<std::string, Channel *>::iterator itMap, Server *server,
             const int &fdUser)
{
    std::string keySetInChannel;
    std::vector<std::string>::iterator it;
    std::string keyToCheck;

    keySetInChannel = itMap->second->getKey();
    if (keySetInChannel.empty() == false && key.empty() == true)
    {
        server->broadcast(fdUser, server->numericReply(server, fdUser,
                                                "475", ERR_BADCHANNELKEY(itMap->second->getChannelName())));
        return (-1);
    }
    if (key.empty() == true)
        return (0);
    it = key.begin() + pos;
    keyToCheck = *it;

    if (keyToCheck.empty() == false && keyToCheck.compare(keySetInChannel) != 0)
    {
        server->broadcast(fdUser, server->numericReply(server, fdUser,
                                                "475", ERR_BADCHANNELKEY(itMap->second->getChannelName())));
        return (-1);
    }
    return (0);
}

int checkChannel(std::string channel, Server *server, const int &fdUser)
{
    std::map<std::string, Channel *>::iterator itMap;

    if (channel.empty() == true)
    {
        server->broadcast(fdUser, server->numericReply(server, fdUser, "461", ERR_NEEDMOREPARAMS(std::string("JOIN"))));
        return (-1);
    }
    if (channel.size() > 50)
    {
        server->broadcast(fdUser, server->numericReply(server, fdUser, "403", ERR_NOSUCHCHANNEL(channel)));
        return (-2);
    }
    if (isChannel(channel) == false || channel.find(',') != std::string::npos)
    {
        server->broadcast(fdUser, server->numericReply(server, fdUser, "476", ERR_BADCHANMASK(channel)));
        return (-3);
    }
    if (server->_channelList.empty() == true)
        return (0);
    itMap = server->_channelList.find(channel);
    if (itMap == server->_channelList.end())
        return (0);
    if (findUserOnChannel(itMap->second->_users, server->getUserByFd(fdUser)) == true)
    {
        server->broadcast(fdUser, server->numericReply(server, fdUser,
            "443", ERR_USERONCHANNEL(server->getUserByFd(fdUser)->getNickname(), channel)));
        return (-4);
    }
    return (0);
}

void addUserToChannel(std::map<std::string, Channel *>::iterator itMap, User *userToAdd)
{
    itMap->second->addUser(userToAdd);
    userToAdd->addChannelJoined(itMap->second->getChannelName());
}


void Server::join(User &user, Command &cmd)
{
    std::vector<std::string> channel;
    std::vector<std::string> key;
    std::vector<std::string>::iterator itChan;
    std::map<std::string, Channel *>::iterator itMap;

    if (cmd.args.size() == 0)
        return (this->broadcast(user.getFd(), server->numericReply(this, user.getFd(),
            "461", ERR_NEEDMOREPARAMS(std::string("JOIN")))));
    channel = splitByComma(cmd.args[0]);
    if (cmd.args.size() > 1)
        key = splitByComma(cmd.args[1]);

    itChan = channel.begin();
    for (; itChan != channel.end(); itChan++)
    {
        if (checkChannel(*itChan, this, user.getFd()) < 0)
            return;
        if (this->_channelList.empty() == false)
        {
            itMap = this->_channelList.find(*itChan);
            if (itMap != this->_channelList.end())
            {
                if (checkKey(itChan - channel.begin(), key, itMap, this, user.getFd()) < 0)
                    return;
                if (itMap->second->hasMode(MOD_INVITE) == true &&
                        findUserOnChannel(itMap->second->_invitees,
                        this->getUserByFd(user.getFd())) == false)
                    return (this->broadcast(user.getFd(), server->numericReply(this, user.getFd(),
                                                                    "473", ERR_INVITEONLYCHAN(*itChan))));
                if (findBannedUserOnChannel(itMap->second->_bannedUsers,
                    this->getUserByFd(user.getFd())->getNickname()) == true
                    && findUserOnChannel(itMap->second->_invitees,
                        this->getUserByFd(user.getFd())) == false)
                    return (this->broadcast(user.getFd(), server->numericReply(this, user.getFd(),
                                                                    "474", ERR_BANNEDFROMCHAN(*itChan))));
                addUserToChannel(itMap, this->getUserByFd(user.getFd()));
            }
            else
                createChannel(*itChan, itChan - channel.begin(), key,
                              this->getUserByFd(user.getFd()), this);
            channelReply(this, user.getFd(), *itChan, cmd.args);
        }
        else
        {
            createChannel(*itChan, itChan - channel.begin(), key,
                          this->getUserByFd(user.getFd()), this);
            channelReply(this, user.getFd(), *itChan, cmd.args);
        }
    }
}