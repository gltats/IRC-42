// This class represents the IRC server itself.
// It's responsible for setting up the server socket,
// accepting new client connections, and managing the overall
// operation of the server.

#pragma once

// epoll and socket includes
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <csignal>

// containers
#include <map>
#include <vector>

// other includes
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <exception>
#include <unistd.h>
#include <iterator>
#include <fcntl.h>
#include <typeinfo>
#include <set>

#define FORBIDDEN_CHARS " ,*?!@$.#&:\r\n\0\a+"
#define MAX_EVENTS 128

#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 20
#define LOSTCONNECTION 0
#define QUITED 1
#define KICKED 2

// Command struct
typedef struct s_command {
	std::string	prefix;
	std::string	cmd;
	std::vector<std::string> args;
} Command;

// class includes
#include "User.hpp"
#include "Channel.hpp"
#include "Logger.hpp"
#include "Parser.hpp"
// #include "Replies.hpp"

class Server
{
private:
    Logger logger;
    // User user;
    // Channel channel;

    int port;
    std::string password;
    int socket;
    int epollFd;
    std::vector<struct epoll_event> epollFds;
    bool maxConnectionsReached;
    std::map<int, User> users;
	std::map<std::string, Channel> channels;

public:
    // cannonical form
    Server(int port, std::string password);
    Server(const Server &server);
    ~Server();

    std::string _hostname;

    // server getters
    int getPort();
    std::string getHostname() const;
    std::string getPassword();
    int getEpollFd() const;
    User *getUserByFd(int fd);
    std::vector<User*> getAllUsers();
    User* getUserByNickname(const std::string& nickname);
    std::vector<Channel*> getChannels();

    // Server setters
    void setPort(int port);
    void setPassword(std::string password);

    // Server setup
    void start();
    void stop();
    bool isServerSocketClosed;
    void setupSocket();
    static Server *instance;
    static void handleSignal(int signal);

    // Connection manager
    std::string receive(int fd);
    void send_data(User &user);
    bool addNewConnection();
    void unexpectedClose(int socket);
    void closeConnection(int userSocket, int reason);
    void removeChannel(std::string name);

    // Events handler
    bool handleServerEvents();
    void handleUserEvents();
    void handleDisconnectionEvents();
    void handleEmptyChannelEvents();

    // Channel manager
    void broadcast(int fd, std::string message);
    std::map<std::string, Channel*>::iterator getChannelName(std::string channelName);
    void removeUserFromChannel(User &user, Channel &channel, std::string message);

    // execute commands && utils
    void executeCommands(User &user, std::vector<Command> &cmd);
    void executeCommand(User &user, Command &cmd);
    std::vector<Command> parseCommands(std::string data);
    Command messageToCommand(std::string source);
    void trim(std::string &s);
    void trimAll(std::string &s);
    void cleanEndOfTransmission(std::string &str);
    bool validChannelName(std::string channelName);

    //Arafa fuctions:
    // std::string	numericReply(Server *irc, const int &fd, std::string code, std::string replyMsg);
    // void informSUsers(Server *srv, std::string msg);
    // void serverQuitNotice(const int &fd,  Server *srv, const std::string &destNick, std::string msg);
    // std::string clientReply(Server *irc, const int &originFd, std::string replyMsg);

    // Commands:
    //  Connection commands
    void pass(User &user, Command &cmd);
    void nick(User &user, Command &cmd);
    void userCommand(User &user, Command &cmd);
    void oper(User &user, Command &cmd);
    void userMode(User &user, Command &cmd);
    void quit(User &user, Command &cmd);
    void join(User &user, Command &cmd);
    void part(User &user, Command &cmd);
    void invite(User &user, Command &cmd);
    void kick(User &user, Command &cmd);
    void topic(User &user, Command &cmd);
    void kill(User &user, Command &cmd);
    void die(User &user, Command &cmd);
    void privmsg(User &user, Command &cmd);

    //Replies:
    std::string welcome(User &user);
    std::string motd(User &user);
    std::string unknowncommand(User &user, std::string cmd);
   
    std::string needmoreparams(User &user, std::string cmd);
    std::string alreadyregistered(User &user);
    std::string passworderror(User &user);
    std::string nooper(User &user);
    std::string operOwnership(User &user);
    std::string channelFull(User &user, std::string channelName);
    std::string inviteOnlyChan(User &user, std::string channelName);
    std::string namreply(User &user, Channel &channel, bool found);
    std::string noChan(User &user, std::string channelName);
    std::string noNick(User &user, std::string nickname);
    std::string notonchannel(User &user, std::string channelName);
    std::string wrongChannelKey(User &user, std::string channelName);
    std::string inviting(User &user, Channel &channel);
    std::string invitedReply(User &user, User &target, Channel &channel);
    std::string kicksuccess(User &user, Channel &ch, std::string target, std::string reason);
    std::string usernotinchannel(User &user, std::string channelName);
    std::string norecipient(User &user, std::string cmd);
    std::string notexttosend(User &user);
    void sucessfulJoin(User &user, Channel &channel);
    std::string chanoprivsneeded(User &user, Channel &channel) ;
    std::string chanoprivsneeded(int fd, Channel &channel);



    // 
    // void user_cmd(User &user, Command &cmd);
    // void mode(User &user, Command &cmd);
    // void oper(User &user, Command &cmd);
    // void quit(User &user, Command &cmd);

    // // Server queries and commands
    // void time(User &user, Command &cmd);
    // void info(User &user, Command &cmd); //checked

    // // Channel command
    // void join(User &user, Command &cmd);//checked
    // void part(User &user, Command &cmd);
    // void invite(User &user, Command &cmd);//checked
    // void kick(User &user, Command &cmd);
    // void topic(User &user, Command &cmd);
    // void list(User &user, Command &cmd);
    // void names(User &user, Command &cmd);

    // // Miscellaneous commands
    // void kill(User &user, Command &cmd);
    // void die(User &user, Command &cmd);//checked

    // // Sending message commands
    // void privmsg(User &user, Command &cmd);
};






