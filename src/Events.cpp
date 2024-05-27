#include "Server.hpp"

bool Server::handleServerEvents()
{
    //check for server events
    struct epoll_event& serverEvent = epollFds.at(0);
    if (serverEvent.events & EPOLLIN && !maxConnectionsReached)
    {
        logger.info("Events", "New server event", logger.getLogTime());

        //add new User
        if (addNewConnection() == false)
        return false;
    }
    return true;
}

void Server::handleUserEvents()
{
    //iterate over user events
    std::vector<struct epoll_event>::iterator it = epollFds.begin() + 1;
    for (; it < epollFds.end(); it++) {
        User &user = users.at((*it).data.fd);
    }
    // handle different types of events
    if((*it).events & EPOLLIN)// EPOLLIN : data to read
    {
        logger.info("Events", "EPOLLIN caught", logger.getLogTime());
        std::string message = receive((*it).data.fd);
        std::vector<Command> commands = parseCommands(message);
		executeCommands(user, commands);
    }
    else if((*it).events & EPOLLOUT)// EPOLLOUT : ready to send data
    {
        send_data(user);
    }
    else if((*it).events & EPOLLERR)// EPOLLERR : error occurred
    {
        logger.error("Events", "EPOLLERR caught", logger.getLogTime());
        unexpectedClose((*it).data.fd);
    }
    else if((*it).events & EPOLLHUP)// EPOLLHUP : hang up
    {
        logger.error("Events", "EPOLLHUP caught", logger.getLogTime());
        closeConnection((*it).data.fd, LOSTCONNECTION);
    }
}

void Server::handleDisconnectionEvents()
{
    //iterate over users in reverse order
    std::map<int, User>::reverse_iterator it = users.rbegin();
    
    //check for disconnection flag
	for (; it != users.rend(); it++) {
		if ((it->second).getDisconnect()) 
        {

            // log disconnection event
			logger.info("Events", "Disconnecting client on fd " + it->first, logger.getLogTime());

            // close the connection
			closeConnection(it->first, QUITED);

            // break after handling
			break;
		}
    }
}

void Server::handleEmptyChannelEvents()
{
     // iterate over channels
    std::map<std::string, Channel*>::iterator it;
    it = channels.begin();
	std::vector<std::string> toDelete;

    
    //empty channels
    for(; it != channels.end(); ++it)
    {
        if (it->second->getUsers().size() == 0)
		    toDelete.push_back(toIrcUpperCase(it->second->getChannelName()));
		it++;
    }

    //remove empty channels
    for(size_t i = 0; i < toDelete.size(); i++)
    {
        logger.info("Events", "Removing empty channel", logger.getLogTime());
        channels.erase(toDelete.at(i));
    }

}

