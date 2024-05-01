#pragma once

#include <string>

class Connection {
private:
    int socket;
    int clientSocket;

public:
    Connection(int socket);
    ~Connection();

    std::string receive();
    void send(const std::string& message);
};