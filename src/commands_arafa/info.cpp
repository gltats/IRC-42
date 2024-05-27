 #include "Server.hpp"

std::string createInfoStr(Server *srv, const int &fd) {
    std::string	replyMsg;
    replyMsg
    .append(srv->numericReply(srv, fd, "371", \
            RPL_INFO(std::string("Version: ").append(VERSION))))//!!Arafa help with macro
    .append(srv->numericReply(srv, fd, "371", \
            RPL_INFO(std::string("Version comment: ").append(VCOMMENT))))//!!Arafa help with macro
    .append(srv->numericReply(srv, fd, "371", \
            RPL_INFO(std::string("Compilation Date: ").append(COMPILDATE))));//!!Arafa help with macro
    return (replyMsg);
}

void info(User &user, Command &cmd)
{
    std::string reply;

    // COMMAND EXECUTION
    try {
        if (cmd.args.size() > 0) {
            if (cmd.args[0].compare(user.getServer().getHostname()) != 0)
                throw std::runtime_error("No such server: " + cmd.args[0]);
        }   
        reply
        .append(createInfoStr(&user.getServer(), user.getFd()))
        .append(user.getServer().numericReply(&user.getServer(), user.getFd(), "374", RPL_ENDOFINFO));
        user.getServer().broadcast(user.getFd(), reply);
    }
    // EXCEPTIONS
    catch (std::runtime_error &e) {
        user.getServer().broadcast(user.getFd(), "ERROR :" + std::string(e.what()));
        return;
    }
}
