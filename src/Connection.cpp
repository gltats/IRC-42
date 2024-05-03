#include "Connection.hpp"

Connection::Connection(int socket) : socket(socket) {}

Connection::~Connection() {}

std::string Connection::receive()
{
    char buffer[1024] = {0};
    int valread = read(socket, buffer, 1024);
    if (valread == 0)
    {
        throw std::runtime_error("Connection closed\n");
    }
    if (valread == -1)
    {
        throw std::runtime_error("Error reading from socket\n");
    }
    return std::string(buffer);
}

void Connection::send_message(const std::string &message)
{
    send(socket, message.c_str(), message.size(), 0);
}