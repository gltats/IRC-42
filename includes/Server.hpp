// This class represents the IRC server itself. 
// It's responsible for setting up the server socket, 
// accepting new client connections, and managing the overall 
// operation of the server. 

#pragma once

// #include "user.hpp"
// #include "channel.hpp"
#include "Logger.hpp"
#include "Connection.hpp"
#include "Channel.hpp"

#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <signal.h>
#include <unistd.h>
#include <iterator>




class Client;
class Channel;
class Connection;

#define FORBIDDEN_CHARS " ,*?!@$.#&:\r\n\0\a+"
#define MAX_EVENTS 128
class Server {
private:
    Logger                      logger;
    int                         port;
    std::string                 password;
    int                         serverSocket;
    int                         epollFd;
    std::vector<Connection*>    connections;
    std::vector<Channel*>       channels;
    std::vector<User*>          users;
    std::vector<struct epoll_event> epollFds;

    // std::vector<User*> users;
    // std::vector<Channel*> channels;
    // std::vector<Connection*> connections;

public:
    //cannonical form
    Server(int port, std::string password);
    Server(const Server& server);
    ~Server();

    // Server setup
    void setupSocket();
    void start();
    void stop();
    static Server* instance;
    static void HandleSignal(int signal);
    bool isServerSocketClosed;

    // Server connection
    void acceptConnection();
    void handleConnection(Connection* connection);
    Connection* findConnectionBySocket(int socket);

    //server getters
    int                         getPort();
    std::string                 getPassword();
    int                         getServerSocket();
    std::vector<User*>          getUsers();
    std::vector<Connection*>    getConnections();
    std::vector<Channel*>       getChannels();
    std::vector<struct epoll_event>& getEpollFds();
    int getEpollFd();
    void getEpollFds(std::vector<struct epoll_event>& epollFds);



    //Server setters
    void setPort(int port);
    void setPassword(std::string password);
    void setServerSocket(int serverSocket);
    void setUsers(std::vector<User*> users);
    void setChannels(std::vector<Channel*> channels);
    void setConnections(std::vector<Connection*> connections);


};