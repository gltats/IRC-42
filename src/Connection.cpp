#include "Server.hpp"

std::string Server::receive(int fd)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesReceived;
    bool reader = true;
    std::string bufferStr = "";

    std::stringstream ss;
    ss << fd;
    std::string socketStr = ss.str();

    logger.info("Connection", "Receiving data from socket " + socketStr, logger.getLogTime());

    while (reader) {
        std::fill(buffer, buffer + BUFFER_SIZE, 0);
        bytesReceived = recv(fd, buffer, BUFFER_SIZE, 0);

        if (bytesReceived <= 0) {
            //here will be unexpectedClose function
            throw std::runtime_error("Connection closed\n");
            reader = false;
        } else {
            bufferStr.append(buffer);
            if (bytesReceived < BUFFER_SIZE) {
                reader = false;
            }
        }
    }

    return bufferStr;
}

void Server::send_data(User &user)
{
    std::stringstream ss;
    ss << socket;
    std::string socketStr = ss.str();

    logger.info("Connection", "Sending data to socket " + socketStr, logger.getLogTime());

    //check from User if the message is available
    int bytesSent;
    std::ostringstream logMessage;
	logMessage << "Sending data to fd " << user.getFd();

    //check if data is available
    if (user.getSendData().size())
    {
        //log attempt to send data
        logger.info("Connection", "Attempting to send data to socket " + socketStr, logger.getLogTime());

        // Send the data
        bytesSent = send(socket, user.getSendData().c_str(), user.getSendData().size(), 0);//!!!

        // Check if the data was sent
        if (bytesSent < 0) {
            logger.error("Connection", "Failed to send data", logger.getLogTime());
        } else {
            logger.info("Connection", "Data sent to socket " + socketStr, logger.getLogTime());
            //reset the message from user class
        }
    }
}

bool Server::addNewConnection() {
    logger.info("addNewConnection", "Adding new connection...", logger.getLogTime());
    
    if (epollFds.size() >= MAX_CONNECTIONS) {
        logger.error("addNewConnection", "The maximum number of connections has been reached. The connection will be rejected", logger.getLogTime());
        maxConnectionsReached = true;
        return false;
    }

    // Accept the connection
    struct sockaddr_in userAddress;
    int addrlen = sizeof(userAddress);
    logger.info("addNewConnection", "Accepting connection...", logger.getLogTime());
    int socket = accept(socket, (struct sockaddr *)&userAddress, (socklen_t *)&addrlen);

    // Check if the connection was successful
    if (socket < 0) {
        logger.error("addNewConnection", "Failed to accept connection", logger.getLogTime());
        return false;
    }

    // Register new client
    User user(socket, inet_ntoa(userAddress.sin_addr));
    users.insert(std::pair<int, User>(socket, user));

    // set up epoll for the new client !!!
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
    ev.data.fd = socket;
    int epollFd = getEpollFd();
    epoll_ctl(epollFd, EPOLL_CTL_ADD, socket, &ev);

    //log the new connection
    std::stringstream ss;
    ss << socket;
    std::string socketStr = ss.str();
    logger.info("Conection", "New connection established with " + user.getHostname() + " on fd " + socketStr, logger.getLogTime());
    return true;
}


void Server::unexpectedClose(int socket)
{
    //retrive user object
    User &user = users.at(socket);
    std::stringstream ss;

    // Log the unexpected disconnection
    logger.info("unexpectedClose", "Handling unexpected disconnection...", logger.getLogTime());

    // Check if the user was registered
    if (user.getStatus() == (NICK_FLAG | USER_FLAG | PASS_FLAG))
    {
        logger.info("Connection", user.getNickname() + ": QUIT: User exited unexpectedly \r\n", logger.getLogTime());

        //remove user from channels and notify other users !!!
        std::vector<Channel *>::iterator it = user.getChannelsJoined().begin();
        for(; it != user.getChannelsJoined().end(); it++)
        {
            (*it)->removeUser(user);
                    std::map<User*, uint>::iterator it2 = (*it)->getUsers().begin();
                    std::map<User*, uint>::iterator it3 = (*it)->getUsers().end();
                    for (; it2 != it3; it2++)
                    {
                        it2->first->setSendData(":" + user.getNickname() + " QUIT\r\n");
                    }
        }
    }
    //disconnect user
    user.setStatus(ST_DISCONNECTED);
}

void Server::closeConnection(int userSocket, int reason) 
{
    //Log the disconnection begining
    logger.info("closeConnection", "Closing connection on fd " + socket, logger.getLogTime());

    //close the socket
    struct epoll_event ev;
    int epollFd = getEpollFd();

    close(userSocket);
    epoll_ctl(epollFd, EPOLL_CTL_DEL, userSocket, &ev);

    // remove the users from the user from UserSocket
    users.erase(userSocket);

    std::ostringstream logReason;
    switch (reason) {
        case LOSTCONNECTION:
            logReason << "Connection lost. (fd : " << userSocket << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
            break;
        case QUITED:
            logReason << "User left. (fd : " << userSocket << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
            break;
        case KICKED:
            logReason << "User kicked. (fd : " << userSocket << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
            break;
        default:
            logReason << "Connection successfully closed. (fd : " << userSocket << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
    }
}

void Server::removeChannel(std::string name) { channels.erase(name); }