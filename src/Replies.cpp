#include "Server.hpp"

std::string Server::welcome(User &user) {
    std::stringstream ss;

    logger.info("welcome", "Sending welcome response to " + user.getNickname(), logger.getLogTime());
	ss << ":localhost 001 " << user.getNickname();
	ss << " :Welcome to the FT_IRC server " << user.getNickname();
	ss << "\r\n";

	ss << ":localhost 002 " << user.getNickname();
	ss << " :Your host is localhost, running version 0.1";
	ss << "\r\n";

	ss << ":localhost 003 " << user.getNickname();
	ss << " :This server was created " << logger.getLogTime();
	ss << "\r\n";

	ss << ":localhost 004 " << user.getNickname();
	ss << " localhost 0.1 iowstRb- biklmnopstvrRcCNuMTD";
	ss << "\r\n";

    return ss.str();
}

std::string Server::motd(User &user) {
    std::stringstream ss;

    logger.info("motd", "Sending motd response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 375 " << user.getNickname();
    ss << " :- localhost Message of the day - ";
    ss << "\r\n";

    ss << ":localhost 372 " << user.getNickname();
    ss << " :- Welcome to the FT_IRC server";
    ss << "\r\n";

    ss << ":localhost 376 " << user.getNickname();
    ss << " :End of /MOTD command.";
    ss << "\r\n";

    return ss.str();
}

std::string Server::unknowncommand(User &user, std::string command) {
    std::stringstream ss;

    logger.info("unknowncommand", "Sending unknowncommand response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 421 " << user.getNickname();
    ss << " " << command << " :Unknown command";
    ss << "\r\n";

    return ss.str();
}

std::string Server::needmoreparams(User &user, std::string command)
{
	std::stringstream ss;

	logger.info("needmoreparams", "Sending needmoreparams response to " + user.getNickname(), logger.getLogTime());

	ss << ":localhost 461 " << user.getNickname();
	ss << " " << command << " :Not enough parameters";
	ss << "\r\n";

	return ss.str();
}

std::string Server::alreadyregistered(User &user) {
	std::stringstream ss;

	logger.info("alreadyregistered", "Sending alreadyregistered response to " + user.getNickname(), logger.getLogTime());
	ss << ":localhost 462 " << user.getNickname();
	ss << " :You may not reregister";
	ss << "\r\n";

	return ss.str();
}

std::string Server::passworderror(User &user) {
    std::stringstream ss;

    logger.info("passworderror", "Sending passworderror response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 464 " << user.getNickname();
    ss << " :Password incorrect";
    ss << "\r\n";

    return ss.str();
}

std::string Server::nooper(User &user) {
    std::stringstream ss;

    logger.info("nooper", "Sending nooperhost response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 491 " << user.getNickname();
    ss << " :No O-lines for your host";
    ss << "\r\n";

    return ss.str();
}

std::string Server::operOwnership(User &user) {
	std::stringstream ss;

	logger.info("operOwnership", "Sending you are the oper response to " + user.getNickname(), logger.getLogTime());
	ss << ":localhost 381 " << user.getNickname();
	ss << " :You are now an IRC operator";
	ss << "\r\n";

	return ss.str();
}

std::string Server::channelFull(User &user, std::string channelName) {
    std::stringstream ss;

    logger.info("channelFull", "Sending channel full response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 471 " << user.getNickname();
    ss << " " << channelName << " :Cannot join channel (+l)";
    ss << "\r\n";

    return ss.str();
}

std::string Server::inviteOnlyChan(User &user, std::string channelName) {
    std::stringstream ss;

    logger.info("inviteOnlyChan", "Sending invite only channel response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 473 " << user.getNickname();
    ss << " " << channelName << " :Cannot join channel (+i)";
    ss << "\r\n";

    return ss.str();
}

void Server::sucessfulJoin(User &user, Channel &channel) {
    std::stringstream ss;

    channel.addUser(user);
    user.addChannel(channel);

    logger.info("sucessfulJoin", "Sending successful join response to " + user.getNickname(), logger.getLogTime());
    
    //join message
    ss << ":" << user.getUserPrefix << " JOIN " << channel.getChannelName() << "\r\n"; 
    channel.broadcast(user, ss.str(), true);

    //handle channel topic
    if (channel.getTopic().length() > 0) 
    {
        ss.str("");
        ss << ":" << user.getNickname() << " TOPIC " << channel.getChannelName() << " :" << channel.getTopic() << "\r\n";
        user.setSendData(ss.str());
    }
    else 
    {
        user.setSendData(":" + user.getNickname() + " 331 " + channel.getChannelName() + " :No topic is set\r\n");
    }

    //broadcast list of users
    channel.broadcast(user, namreply(user, channel, true), true);

}

std::string Server::namreply(User &user, Channel &channel, bool found) {
    std::stringstream ss;

    logger.info("namreply", "Sending namreply response to " + user.getNickname(), logger.getLogTime());

    if(found)
    {
        User *creator = channel.getCreator();
        ss << ":localhost 353 " << user.getNickname();
        ss << " = " << channel.getChannelName() << " :";

        //remove creator to dont have extra user
        if (creator)
        {
            ss << "!" << creator->getNickname() << " ";
        }

        std::map<User *, unsigned int>::iterator it = channel.getUsers().begin();
        while(it != channel.getUsers().end())
        {
            if (it->second & CH_OPERATOR)
                ss << "@";
            ss << it->first->getNickname() << " ";
            it++;
        }
        ss << "\r\n";
    }
    ss << ":localhost 366 " << user.getNickname();
    ss << " " << channel.getChannelName() << " :End of /NAMES list";
    ss << "\r\n";

    return ss.str();
}

std::string Server::noChan(User &user, std::string channelName) {
    std::stringstream ss;

    logger.info("noChan", "Sending no channel response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 403 " << user.getNickname();
    ss << " " << channelName << " :No such channel";
    ss << "\r\n";

    return ss.str();
}

std::string Server::noNick(User &user, std::string nickname) {
    std::stringstream ss;

    logger.info("noNick", "Sending no nickname response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 401 " << user.getNickname();
    ss << " " << nickname << " :No such nick/channel";
    ss << "\r\n";

    return ss.str();
}

std::string Server::notonchannel(User &user, std::string channelName) {
    std::stringstream ss;

    logger.info("notonchannel", "Sending not on channel response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 442 " << user.getNickname();
    ss << " " << channelName << " :You're not on that channel";
    ss << "\r\n";

    return ss.str();
}

std::string Server::wrongChannelKey(User &user, std::string channelName) {
    std::stringstream ss;

    logger.info("wrongChannelKey", "Sending wrong channel key response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 475 " << user.getNickname();
    ss << " " << channelName << " :Cannot join channel (+k)";
    ss << "\r\n";

    return ss.str();
}

std::string Server::inviting(User &user, Channel &channel) {
    std::stringstream ss;

    logger.info("inviting", "Sending inviting response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 341 " << user.getNickname();
    ss << " " << channel.getChannelName() << " " << user.getNickname();
    ss << "\r\n";

    return ss.str();
}

std::string Server::invitedReply(User &user, User &target, Channel &channel) {
    std::stringstream ss;

    logger.info("invitedReply", "Sending invited reply response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 341 " << target.getNickname();
    ss << " " << channel.getChannelName() << " " << user.getNickname();
    ss << "\r\n";

    return ss.str();
}

std::string Server::kicksuccess(User &user, Channel &ch, std::string target, std::string reason) {
    std::stringstream ss;

    logger.info("kicksuccess", "Sending kick success response to " + user.getNickname(), logger.getLogTime());
    ss << ":" << user.getNickname() << " KICK " << ch.getChannelName() << " " << target << " :" << reason << "\r\n";

    return ss.str();
}

std::string Server::usernotinchannel(User &user, std::string channelName) {
    std::stringstream ss;

    logger.info("usernotinchannel", "Sending user not in channel response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 441 " << user.getNickname();
    ss << " " << channelName << " :They aren't on that channel";
    ss << "\r\n";

    return ss.str();
}

std::string Server::norecipient(User &user, std::string command) {
    std::stringstream ss;

    logger.info("norecipient", "Sending no recipient response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 411 " << user.getNickname();
    ss << " " << command << " :No recipient given (" << command << ")";
    ss << "\r\n";

    return ss.str();
}

std::string Server::notexttosend(User &user) {
    std::stringstream ss;

    logger.info("notexttosend", "Sending no text to send response to " + user.getNickname(), logger.getLogTime());
    ss << ":localhost 412 " << user.getNickname();
    ss << " :No text to send";
    ss << "\r\n";

    return ss.str();
}

std::string Server::chanoprivsneeded(User &user, Channel &channel) {
	std::stringstream ss;

	logger.info("chanoprivsneeded", "Sending chanoprivsneeded response to " + user.getNickname(), logger.getLogTime());
	ss << ":localhost 482";
	ss << " " << user.getNickname();
	ss << " " << channel.getChannelName();
	ss << " :You're not channel operator";
	ss << "\r\n";

	return ss.str();
}

std::string Server::chanoprivsneeded(int fd, Channel &channel) {
    std::stringstream ss;

    // convert fd to string using stringstream
    std::stringstream fd_ss;
    fd_ss << fd;
    std::string fdAsString = fd_ss.str();

    logger.info("chanoprivsneeded", "Sending channel operator privs needed response to " + fdAsString, logger.getLogTime());
    ss << ":localhost 482 " << fd;
    ss << " " << channel.getChannelName() << " :You're not channel operator";
    ss << "\r\n";

    return ss.str();
}