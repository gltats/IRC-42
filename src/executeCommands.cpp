#include "Server.hpp"

void Server::executeCommands(User &user, std::vector<Command> &cmd)
{
    // logging the start of command execution
	logger.info("executeCommands", "Executing commands", logger.getLogTime());

    // initialize nickname for logging
	std::string nick = "(unidentified)";

    // interate over commands
	std::vector<Command>::iterator it = cmd.begin();

	for (; it != cmd.end(); it++) 
    {
        // update nickname if identified
		if (user.getNickname().size() > 0) {
			nick = user.getNickname();
		}
        
        // log each command
		logger.info("executeCommands", "User " + nick + " on fd " + std::to_string(user.getFd()) + " sent: " + (*it).cmd , logger.getLogTime());

        // execute individual command
		executeCommand(user, (*it));
    }
}

void Server::executeCommand(User &user, Command &cmd)
{
    //command execution
    if (cmd.cmd == "PASS") {
		pass(user, cmd);
	} else if (cmd.cmd == "NICK") {
		nick(user, cmd);
	} else if (cmd.cmd == "USER") {
		user_cmd(user, cmd);
    } else if (cmd.cmd == "MODE") {
		mode(user, cmd);
	} else if (cmd.cmd == "OPER") {
		oper(user, cmd);
    } else if (cmd.cmd == "QUIT") {
		quit(user, cmd);
    } else if (cmd.cmd == "TIME") {
		time(user, cmd);
    } else if (cmd.cmd == "INFO") {
		info(user, cmd);
	} else if (cmd.cmd == "JOIN") {
		join(user, cmd);
	} else if (cmd.cmd == "PART") {
		part(user, cmd);
	} else if (cmd.cmd == "INVITE") {
		invite(user, cmd);
    } else if (cmd.cmd == "KICK") {
		kick(user, cmd);
	} else if (cmd.cmd == "TOPIC") {
		topic(user, cmd);
    } else if (cmd.cmd == "LIST") {
		topic(user, cmd);
    } else if (cmd.cmd == "NAMES") {
        names(user, cmd);
    } else if (cmd.cmd == "KILL") {
		kill(user, cmd);
    } else if (cmd.cmd == "DIE") {
        die(user, cmd);
	} else if (cmd.cmd == "PRIVMSG") {
		privmsg(user, cmd);

    // } else if (cmd.cmd == "PING") {
	// 	ping(user, cmd);
	// } else if (cmd.cmd == "NOTICE") {
	// 	notice(user, cmd);
	// } else if (cmd.cmd == "WHO") {
	// 	who(user, cmd);
	// } else if (cmd.cmd == "WHOIS") {
	// 	whois(user, cmd);


	} else {
		// user.setSendData(unknowncommand(client, command.cmd));
		user.setSendData(ERR_UNKNOWNCOMMAND);// !!Arafa need help here,  need to use the MACROS from includes/Replies.hpp
	}

    // check if user is registered and send welcome message
    if (user.getStatus() == (PASS_FLAG | USER_FLAG | NICK_FLAG) && !user.getWelcome())
    {
		user.setWelcome(true);
		user.setSendData(welcome(user)); //!!Arafa need help here, need to use the MACROS from includes/Replies.hpp
		user.setSendData(motd(user)); //!!Arafa need help here,  need to use the MACROS from includes/Replies.hpp
	}
}

std::string	Server::numericReply(Server *irc, const int &fd, std::string code, std::string replyMsg)
{
	std::string reply = ":" + irc->getHostname() + " " + code + " "
						+ irc->getUserByFd(fd)->getNickname() + " " + replyMsg;		
	return (reply);
}

std::string Server::clientReply(Server *irc, const int &originFd, std::string replyMsg)
{
	std::string reply = ":" + irc->getUserByFd(originFd)->getNickname() + "!"
						+ irc->getUserByFd(originFd)->getUsername() + "@:"
						+ irc->getUserByFd(originFd)->getHostname() + " " + replyMsg
                        + "\r\n";
	return (reply);
}

void	Server::serverQuitNotice(const int &fd,  Server *srv, const std::string &destNick, std::string msg) 
{
	std::string reply = ":" + srv->getHostname() + " NOTICE " + destNick + " " + ":" + msg;
	srv->broadcast(fd, reply);
}

void 	Server::informSUsers(Server *srv, std::string msg) 
{
    std::vector<User*> users = srv->getAllUsers();
    for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it) {
        User* user = *it;
        if (user->hasMode(MOD_SRVNOTICES))
            serverQuitNotice(user->getFd(), srv, user->getNickname(), msg);
    }
}