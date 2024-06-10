// This class represents an IRC channel. It's responsible for 
// managing a group of Users that are joined to the channel and 
// broadcasting messages to all users in the channel.

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <algorithm>

#include "User.hpp"
#include "Logger.hpp"
// #include "Replies.hpp"
#include "Parser.hpp"

//   The available modes are as follows:
//		none ?												=> 0	0000 0000
//		o - channel operator;						        => 1	0000 0001
//     	k - key locked;                  					=> 3	0000 0010
//		i - invite only;        							=> 3	0000 0100
//      b - ban user;         								=> 4	0000 1000

#define CHANNEL_PREFIX "&#!+"
#define USER_CHANNEL_LIMIT 10
#define CH_OPERATOR 0b1000
#define CH_SPEAKER 0b10000
// #define MOD_KEY (1 << 2)
// #define MOD_INVITE (1 << 3)
// #define MOD_BAN (1 << 4)

class User; 

class Channel
{
    private:
        Logger logger;
        std::string _channelName;
        std::string _key;
        std::string _topic;
        std::string	host;
        std::set<char> modes;
        unsigned int userLimit;
        std::map<User *, unsigned int> _users;
        bool initialized;
        User *creator;
        std::set<std::string> invited;
    public:
        Channel(void);
        Channel(std::string name, User *creator);
        ~Channel(void);
        Channel &operator=(Channel const &cpy);

        //getters
        std::string getChannelName(void) const;
        std::string getTopic(void) const;
        User *getCreator(void) const;
        std::set<char> &getMode();
        unsigned int getUserLimit() const;
        std::map<User *, unsigned int> &getUsers(void);
        std::string getStrModes();
        std::map<User *, unsigned int>::iterator getUserByNickname(std::string nickname);

        //setters
        void setTopic(std::string topic);
        void setKey(std::string key);
        void setUserLimit(unsigned int limit);
        bool setOperator(std::string nickname, bool newValue);
        void setSpeaker(std::string nickname, bool newValue);
        
        
        bool isInitialized(void);
        void addUser(User &user);
        void removeKey(void);
        void removeUser(User &user);
        void broadcast(User &sender, std::string message, bool toSend);
        bool manageMode(char mode, bool on);
        void initializes(std::string channelName, std::string key, User &op);
        void initialize(std::string channelName, User &op);
        bool checkKey(std::string key);
        bool isInvited(std::string nickname);
        void inviteUser(std::string nickname);
        void uninviteUser(std::string nickname);
        void makeOperator(void);
};


