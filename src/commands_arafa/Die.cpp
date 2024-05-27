#include "Server.hpp"

void die(User &user, Command &cmd)
{
    Server *srv = &user.getServer();
    int fd = user.getFd();

    if (user.hasMode(MOD_OPER)) {
        srv->informSUsers(srv, "Server is shutting down");
        throw std::runtime_error("Server is shutting down");
    }
    else {
        srv->broadcast(fd, srv->numericReply(srv, fd, "481", ERR_NOPRIVILEGES));
    }
}