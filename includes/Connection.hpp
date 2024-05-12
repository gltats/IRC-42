#pragma once

#include <string>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "Logger.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Server.hpp"

#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 20
#define LOSTCONNECTION 0
#define QUITED 1
#define KICKED 2

class Connection {
private:
    int socket;
    int clientSocket;
    Server& server;
    Logger& logger;
    User& user;
    
    bool maxConnectionsReached;
    std::vector<Channel*> channels;


public:
    Connection(int socket);
    ~Connection();

    int getSocket();

    std::string receive();
    void send_message(const std::string& message);

    //connection manager functions
    bool addNewConnection();
    void unexpectedClose(int socket);
    void closeConnection(int UserSocket, int reason);
    void removeChannel(std::string name);
};