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
#include <map>

#include "Logger.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Server.hpp"

#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 20
#define LOSTCONNECTION 0
#define QUITED 1
#define KICKED 2

class Server;

class Connection {
private:
    int socket;
    int UserSocket;
    Logger logger;
    Server& server;
    Channel channel;
    User user;
    
    bool maxConnectionsReached;
	std::map<int, User> users;
	std::map<std::string, Channel> channels;

public:
    Connection(int socket, Server& server);
    ~Connection();

    int getSocket();
    Channel* getChannelByName(std::string name);

    //set maps to be used in another class
    void setCUsers(std::map<int, User> users);
    void setCChannels(std::map<std::string, Channel> channels);

    std::string receive();
    void send_data(const std::string& message);

    //connection manager functions
    bool addNewConnection();
    void unexpectedClose(int socket);
    void closeConnection(int UserSocket, int reason);
    void removeChannel(std::string name);
};