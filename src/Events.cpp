#include "Events.hpp"

Events::Events() : _serverEvents(false), _userEvents(false), _disconnectionEvents(false), _emptyChannelEvents(false), logger(logger), server(server), connection(connection), user(user) {}

Events::Events(Events const &src) : _serverEvents(src._serverEvents), _userEvents(src._userEvents), _disconnectionEvents(src._disconnectionEvents), _emptyChannelEvents(src._emptyChannelEvents), logger(src.logger), server(src.server), connection(src.connection), user(src.user){}

Events::~Events() {}

Events &Events::operator=(Events const &cpy)
{
    if (this != &cpy)
    {   
        this->_serverEvents = cpy._serverEvents;
        this->_userEvents = cpy._userEvents;
        this->_disconnectionEvents = cpy._disconnectionEvents;
        this->_emptyChannelEvents = cpy._emptyChannelEvents;
    }
    return *this;
}

bool Events::handleServerEvents()
{
    //check for server events
    std::vector<struct epoll_event>& epollFds = server.getEpollFds();
    bool maxUsersFlag = server.getMaxUsersFlag();
    if (epollFds[0].events & EPOLLIN && !maxUsersFlag)
    {
         _serverEvents = true;
        
        logger.info("Events", "New server event", logger.getLogTime());

        //add new User
        if (connection.addNewConnection() == false)
        return false;
    }
    return true;
}

void Events::handleUserEvents()
{
    //iterate over user events
    std::vector<User*> users = server.getUsers();
    std::vector<struct epoll_event>::iterator it = server.getEpollFds().begin() + 1;
    for(; it < server.getEpollFds().end(); ++it)
    {
        User *user = users.at(it->data.fd);
    }
    // handle different types of events
    
}