#include "../../includes/commands.hpp"
#include "../../includes/utils.hpp"
#include "../../includes/Server.hpp"

void pass(const int &fd, const std::vector<std::string> &params, const std::string &, Server *srv) 
{   
    std::cout << "PASS command received. Params: ";
    for (size_t i = 0; i < params.size(); ++i) {
        std::cout << params[i] << " ";
    }
    std::cout << std::endl;

    std::string replyMsg;
    User *user = srv->getUserByFd(fd);

    if (user != 0)
    {
        if (params.empty() || params[0].empty()) {
            std::cout << "No password provided with PASS command." << std::endl;
            replyMsg = numericReply(srv, fd, "461", ERR_NEEDMOREPARAMS(std::string("PASS")));
        }
        else if (user->getPassword() == true) {
            std::cout << "User already registered." << std::endl;
            replyMsg = numericReply(srv, fd, "462", ERR_ALREADYREGISTRED); 
        }
        else if (!user->getPassword() && srv->getPassword() == params[0]) {   
            std::cout << "Password matched. Setting password for user." << std::endl;
            user->setPassword(true);
        }
        else {
            std::cout << "Password did not match." << std::endl;
        }
    }
    else {
        std::cout << "No user found for fd: " << fd << std::endl;
    }

    srv->sendClient(fd, replyMsg);
    return ;
}