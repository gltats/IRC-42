// This class represents an IRC channel. It's responsible for 
// managing a group of Users that are joined to the channel and 
// broadcasting messages to all users in the channel.

#pragma once

#include <string>
#include <vector>
#include "user.hpp"

class Channel {
private:
    std::string name;
    std::vector<User*> users;

public:
    Channel(std::string name);
    ~Channel();

    void addUser(User &user);
    void removeUser(User &user);
    void broadcastMessage(std::string message);
};