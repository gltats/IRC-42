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

