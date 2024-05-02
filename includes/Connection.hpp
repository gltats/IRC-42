#pragma once

#include <string>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

class Connection {
private:
    int socket;
    int clientSocket;

public:
    Connection(int socket);
    ~Connection();

    int getSocket();

    std::string receive();
    void send_message(const std::string& message);
};