#include "Server.hpp"

void Server::broadcast(int fd, std::string message) {

    // logging the broadcast message
	logger.info("broadcastMessage", "Broadcasting message to all Users except fd " + fd, logger.getLogTime());

    // iterating over Users
	std::map<int, User>::iterator it = users.begin();
	for (; it != users.end(); it++) {
		if ((it->first) != fd) {
			(it->second).setSendData(message);
		}
	}
}

std::map<std::string, Channel>::iterator Server::getChannelName(std::string channelName) {
    // convert channel name to UpperCase
    std::string upperInput = toIrcUpperCase(channelName);

    // logging the search
    logger.info("getChannelByName", "Looking for channel " + upperInput, logger.getLogTime());

    // iterate over channels
    std::map<std::string, Channel>::iterator it = channels.begin();
    while (it != channels.end()) {
        std::string upperChannel = toIrcUpperCase(it->first);
        if (upperInput == upperChannel) {
            return it;
        }
        it++;
    }

    // return channel's end if not found
    return channels.end();
}

void Server::removeUserFromChannel(User &user, Channel &channel, std::string message) {
    // logging the removal
	std::ostringstream logMessage;
	logMessage << "Removing user " << user.getNickname() << " from channel " << channel.getChannelName();
	logger.info("removeUserFromChannel", "Removing user " + user.getNickname() + " from channel " + channel.getChannelName(), logger.getLogTime());

    //removing user from channel !!!
	channel.removeUser(user);
	user.removeChannel(&channel);

    // channel empty check
	if (channel.getUsers().size() == 0) {
		this->channels.erase(toIrcUpperCase(channel.getChannelName()));
	} else { // Broadcasting Part message
		std::stringstream ss;

		ss << ":";
		ss << user.getNickname();
		ss << " PART";
		ss << " " << channel.getChannelName();
		ss << " " << message;
		ss << "\r\n";

		channel.broadcast(user, ss.str(), false);
	}
}