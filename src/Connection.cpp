#include "Connection.hpp"
#include <sstream>

Connection::Connection(int socket, Server& server): socket(socket), UserSocket(UserSocket), logger(logger), server(server), maxConnectionsReached(false), channel(channel), user(user)  {}

Connection::~Connection() {}

std::string Connection::receive()
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesReceived;
    bool reader = true;
    std::string bufferStr = "";

    logger.info("Connection", "Receiving data from socket " + std::to_string(socket), logger.getLogTime());

    while (reader) {
        std::fill(buffer, buffer + BUFFER_SIZE, 0);
        bytesReceived = recv(socket, buffer, BUFFER_SIZE, 0);

        if (bytesReceived <= 0) {
            //here will be unexpectedClose function
            throw std::runtime_error("Connection closed\n");
        } else {
            bufferStr.append(buffer);
            if (bytesReceived < BUFFER_SIZE) {
                reader = false;
            }
        }
    }

    return bufferStr;
}

void Connection::send_message(const std::string &message)
{
    logger.info("Connection", "Sending data to socket " + std::to_string(socket), logger.getLogTime());

    //before should check from Usser if the message is available
    int bytesSent = send(socket, message.c_str(), message.size(), 0);

    if (bytesSent < 0) {
        throw std::runtime_error("Failed to send data\n");
    } else {
        logger.info("Connection", "Data sent to socket " + std::to_string(socket), logger.getLogTime());
        //reset the message from user class
    }
}

int Connection::getSocket() {
    return socket;
}

bool Connection::addNewConnection() {
    logger.info("addNewConnection", "Adding new connection...", logger.getLogTime());
    
    std::vector<struct epoll_event>& epollFds = server.getEpollFds();
    if (epollFds.size() >= MAX_CONNECTIONS) {
        logger.error("addNewConnection", "The maximum number of connections has been reached. The connection will be rejected", logger.getLogTime());
        maxConnectionsReached = true;
        return false;
    }

    // Accept the connection
    struct sockaddr_in userAddress;
    int addrlen = sizeof(userAddress);
    logger.info("addNewConnection", "Accepting connection...", logger.getLogTime());
    int UserSocket = accept(socket, (struct sockaddr *)&userAddress, (socklen_t *)&addrlen);

    // Check if the connection was successful
    if (UserSocket < 0) {
        logger.error("addNewConnection", "Failed to accept connection", logger.getLogTime());
        return false;
    }

    // Register new client
    User user(socket, inet_ntoa(userAddress.sin_addr));
    users.insert(std::pair<int, User>(socket, user));

    // set up epoll for the new client
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
    ev.data.fd = UserSocket;
    int epollFd = server.getEpollFd();
    epoll_ctl(epollFd, EPOLL_CTL_ADD, UserSocket, &ev);

    //log the new connection
    std::ostringstream logMessage;
    logMessage << "New connection established with " << user.getHostname() << " on fd " << UserSocket;
    logger.info("Conection", logMessage.str(), logger.getLogTime());
    return true;
}

Channel* Connection::getChannelByName(std::string name) {
    // Iterate over your collection of channels
    std::map<std::string, Channel>::iterator it = channels.find(name);
    if (it != channels.end()) {
        return &(it->second);
    }
    return NULL;
}

void Connection::unexpectedClose(int socket) {
    //retrive user object
    User &user = users.at(UserSocket);
    std::stringstream ss;

    // Log the unexpected disconnection
    logger.info("unexpectedClose", "Handling unexpected disconnection...", logger.getLogTime());

    // Check if the user was registered
    if (user.getStatus() == (NICK_FLAG | USER_FLAG | PASS_FLAG))
    {
        std::stringstream ss;
        ss << ":" << user.getNickname();
        ss << " QUIT: Client exited unexpectedly";
        ss << "\r\n";
        logger.info("Connection", ss.str(), logger.getLogTime());

        //remove user from channels and notify other users
        std::deque<std::string>::iterator it = user.getChannelsJoined().begin();
        for (; it != user.getChannelsJoined().end(); it++)
        {
            // You need to get the Channel* from the string here
            Channel* channel = getChannelByName(*it);
            if (channel) {
                channel->removeUser(&user);
                std::deque<User*>::iterator itb = channel->getUsers().begin();
                std::deque<User*>::iterator ite = channel->getUsers().end();
                for (; itb != ite; itb++) {
                    (*itb)->setSendData(ss.str());
                }
            }
        }
    }
    user.setStatus(ST_DISCONNECTED);
}

void Connection::closeConnection(int UserSocket, int reason) 
{
    //Log the disconnection begining
    std::ostringstream logMessage;
    logMessage << "Closing connection on fd " << UserSocket;
    logger.info("closeConnection", logMessage.str(), logger.getLogTime());

    // iterate through the pollFds vector to find the user socket
    struct epoll_event ev;
    std::vector<struct epoll_event>& epollFds = server.getEpollFds();
    int epollFd = server.getEpollFd();

    std::vector<struct epoll_event>::iterator it = epollFds.begin();
    for (; it < epollFds.end(); it++) {
        if ((*it).data.fd == UserSocket) {
            close(UserSocket);
            epoll_ctl(epollFd, EPOLL_CTL_DEL, UserSocket, &ev);
            epollFds.erase(it);
            break;
        }
    }

    // remove the users from the user from UserSocket
    users.erase(UserSocket);

    std::ostringstream logReason;
    switch (reason) {
        case LOSTCONNECTION:
            logReason << "Connection lost. (fd : " << UserSocket << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
            break;
        case QUITED:
            logReason << "User left. (fd : " << UserSocket << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
            break;
        case KICKED:
            logReason << "User kicked. (fd : " << UserSocket << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
            break;
        default:
            logReason << "Connection successfully closed. (fd : " << UserSocket << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
    }
}

void Connection::removeChannel(std::string name) { channels.erase(name); }