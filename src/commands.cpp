#include "Server.hpp"

void Server::pass(User &user, Command &cmd) {

    // logging the authentication attempt
	logger.info("pass", "user " + user.getNickname() + " is trying to authenticate", logger.getLogTime());

    // check for password arguments
	if (cmd.args.size() == 0) {
		user.setSendData(needmoreparams(user, "PASS"));
		return;
	} else if (user.getStatus() & PASS_FLAG) { // check for previous authentication
		user.setSendData(alreadyregistered(user));
		return;
	} else if (cmd.args[0] != password) { // password validation
		user.setSendData(passworderror(user));
		return;
	}

    // setting user as authenticated
	user.setPassword(true);
	user.setStatus(PASS_FLAG);
}
