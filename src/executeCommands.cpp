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
        
		std::stringstream ss;
		ss << user.getFd();
		std::string fdAsString = ss.str();

		// log each command
		logger.info("executeCommands", "User " + nick + " on fd " + fdAsString + " sent: " + (*it).cmd , logger.getLogTime());

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
		userCommand(user, cmd);
    } else if (cmd.cmd == "MODE") {
		userMode(user, cmd);
	} else if (cmd.cmd == "OPER") {
		oper(user, cmd);
    } else if (cmd.cmd == "QUIT") {
		quit(user, cmd);
    // } else if (cmd.cmd == "TIME") {
	// 	time(user, cmd);
    // } else if (cmd.cmd == "INFO") {
	// 	info(user, cmd);
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
    // } else if (cmd.cmd == "LIST") {
	// 	topic(user, cmd);
    // } else if (cmd.cmd == "NAMES") {
    //     names(user, cmd);
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
		user.setSendData(unknowncommand(user, cmd.cmd));
	}

    // check if user is registered and send welcome message
    if (user.getStatus() == (PASS_FLAG | USER_FLAG | NICK_FLAG) && !user.getWelcome())
    {
		user.setWelcome(true);
		user.setSendData(welcome(user)); //!!Arafa need help here, need to use the MACROS from includes/Replies.hpp
		user.setSendData(motd(user)); //!!Arafa need help here,  need to use the MACROS from includes/Replies.hpp
	}
}