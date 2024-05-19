#pragma once

#include <iostream>


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
};