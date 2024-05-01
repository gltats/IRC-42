//This class represents a user of the IRC server. 
// It's responsible for storing the user's nickname, username, and 
// password, and for authenticating the user.

#pragma once   

#include <string>

class User {
private:
    std::string nickname;
    std::string username;
    std::string password;

public:
    User(std::string nickname, std::string username, std::string password);
    ~User();

    std::string getNickname();
    std::string getUsername();
    std::string getPassword();

    bool authenticate(std::string password);
};

    