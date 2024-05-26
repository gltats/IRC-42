#include "Server.hpp"

int checkParameterInvite(User &user, Command &cmd, Server *server, User *userToInvite)
{
    std::string nickname = cmd.args[0];
    std::string channel = cmd.args[1];

    // Nickname and channel must not be empty
    if (nickname.empty() == true || channel.empty() == true)
    {
        server->broadcast(user.getFd(), server->numericReply(server, user.getFd(),
                                                "461", ERR_NEEDMOREPARAMS(std::string("INVITE"))));
        return (-1);
    }
    // Channel list must not be empty
    if (server->channels.empty() == true)
    {
        server->broadcast(user.getFd(), server->numericReply(server, user.getFd(),
                                                "403", ERR_NOSUCHCHANNEL(channel)));
        return (-1);
    }
    // User must exist
    if (server->getUserByNickname(nickname) == NULL)
    {
        server->broadcast(user.getFd(), server->numericReply(server, user.getFd(),
                                                "401", ERR_NOSUCHNICK(nickname)));
        return (-2);
    }
    // According to RFC "There is no requirement that the
    // channel the target user is being invited to must exist or be a valid
    // channel". We chose to have an existing channel as a requirement
    std::map<std::string, Channel*>::iterator it = server->channels.find(channel);
    if (it == server->channels.end())
    {
        server->broadcast(user.getFd(), server->numericReply(server, user.getFd(),
                                                "403", ERR_NOSUCHCHANNEL(channel)));
        return (-3);
    }
    // User is already on channel
    if (server->findUserOnChannel(it->second->_users, server->getUserByNickname(nickname)) == true)
    {
        server->broadcast(user.getFd(), server->numericReply(server, user.getFd(),
                                                "443", ERR_USERONCHANNEL(userToInvite->getFullname(), channel)));
        return (-4);
    }
    // Current user must be in channel
    if (server->findUserOnChannel(it->second->_users,server->getUserByFd(user.getFd())) == false)
    {
        server->broadcast(user.getFd(), server->numericReply(server, user.getFd(),
                                                "442", ERR_NOTONCHANNEL(channel)));
        return (-5);
    }
    // Current user must be an operator if channel is invite only
    if (it->second->hasMode(MOD_INVITE) && server->findUserOnChannel(it->second->_operators,
                                                               server->getUserByFd(user.getFd())) == false)
    {
        server->broadcast(user.getFd(), server->numericReply(server, user.getFd(),
                                                "482", ERR_CHANOPRIVSNEEDED(channel)));
        return (-6);
    }
    return (0);
}

void Server::invite(User &user, Command &cmd)
{
    std::string nickname;
    std::string channel;
    User *userToInvite;
    Channel *channelPos;

    if (cmd.args.size() < 2)
        return (user.getServer().broadcast(user.getFd(), numericReply(&user.getServer(), user.getFd(),
            "461", ERR_NEEDMOREPARAMS(std::string("INVITE")))));
    nickname = cmd.args[0];
    channel = cmd.args[1];
    userToInvite = user.getServer().getUserByNickname(nickname);
    if (checkParameterInvite(user, cmd, &user.getServer(), userToInvite) < 0)
        return;
    // Add user to the list of invitee and return reply
    channelPos = user.getServer().channels.find(channel)->second;
    channelPos->addInvitee(user.getServer().getUserByNickname(nickname));
    user.getServer().broadcast(user.getServer().getUserByNickname(nickname)->getFd(),
        clientReply(&user.getServer(), user.getFd(), "INVITE " +
            nickname + " " + channel));
}