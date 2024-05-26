#include "Server.hpp"

std::string Server::receive(int fd)
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
    logger.info("Connection", "Sending data to socket " + std::to_string(socket), logger.getLogTime());

    //check from User if the message is available
    int bytesSent;
    std::ostringstream logMessage;
	logMessage << "Sending data to fd " << user.getFd();

    //check if data is available
    if (user.getSendData().size())
    {
        //log attempt to send data
        logger.info("Connection", "Attempting to send data to socket " + std::to_string(socket), logger.getLogTime());

        // Send the data
        bytesSent = send(socket, user.getSendData().c_str(), user.getSendData().size(), 0);//!!!

        // Check if the data was sent
        if (bytesSent < 0) {
            logger.error("Connection", "Failed to send data", logger.getLogTime());
        } else {
            logger.info("Connection", "Data sent to socket " + std::to_string(socket), logger.getLogTime());
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
    logger.info("Conection", "New connection established with " + user.getHostname() + " on fd " + std::to_string(socket), logger.getLogTime());
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
        // std::vector<Channel *>::iterator it = user.getChannels().begin();
		// for (; it != user.getChannels().end(); it++) 
        // {
		// 	(*it)->removeUser(user);
		// 	std::map<Client *, uint>::iterator itb = (*it)->getUsers().begin();
		// 	std::map<Client *, uint>::iterator ite = (*it)->getUsers().end();
		// 	for (; itb != ite; itb++)
        //     {
		// 		itb->first->setSendData(ss.str());
		//     }
        // }
    }
    //disconnect user
    user.setStatus(ST_DISCONNECTED);
}

void Server::closeConnection(int userFd, int reason) 
{
    //Log the disconnection begining
    logger.info("closeConnection", "Closing connection on fd " + socket, logger.getLogTime());

    //close the socket
    struct epoll_event ev;
    int epollFd = getEpollFd();

    close(userFd);
    epoll_ctl(epollFd, EPOLL_CTL_DEL, userFd, &ev);

    // remove the users from the user from UserSocket
    users.erase(userFd);

    std::ostringstream logReason;
    switch (reason) {
        case LOSTCONNECTION:
            logReason << "Connection lost. (fd : " << userFd << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
            break;
        case QUITED:
            logReason << "User left. (fd : " << userFd << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
            break;
        case KICKED:
            logReason << "User kicked. (fd : " << userFd << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
            break;
        default:
            logReason << "Connection successfully closed. (fd : " << userFd << ")";
            logger.info("closeConnection", logReason.str(), logger.getLogTime());
    }
}

void Server::removeChannel(std::string name) { channels.erase(name); }