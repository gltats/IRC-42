#pragma once

#include <iostream>
#include "Logger.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Server.hpp"

class Server;
class User;

class Events
{
    public:
        Events();
        Events(Events const &src);
        ~Events();
        Events &operator=(Events const &cpy);

        bool handleServerEvents();
	    void handleUserEvents();
	    void handleDisconnectionEvents();
	    void handleEmptyChannelEvents();

    private:
        bool _serverEvents;
        bool _userEvents;
        bool _disconnectionEvents;
        bool _emptyChannelEvents;
        Logger logger;
        Server& server;
        Connection& connection;
        User& user;
};