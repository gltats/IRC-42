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

void Server::nick(User &user, Command &cmd) {
	// logging the nickname change
	logger.info("nick", "user " + user.getNickname() + " is trying to change nickname", logger.getLogTime());

	// check for nickname arguments
	if (cmd.args.size() == 0) {
		user.setSendData(needmoreparams(user, "NICK"));
		return;
	} else if (user.getStatus() & NICK_FLAG) { // check for previous nickname
		user.setSendData(alreadyregistered(user));
		return;
	}

	// setting user nickname
	user.setNickname(cmd.args[0]);
	user.setStatus(NICK_FLAG);
}

void Server::userCommand(User &user, Command &cmd) {
	// logging the user registration
	logger.info("user", "user " + user.getNickname() + " is trying to register", logger.getLogTime());

    // check for sufficient arguments
	if (cmd.args.size() < 4) {
		user.setSendData(needmoreparams(user, "USER"));
		return;
	} else if (user.getStatus() & USER_FLAG) { // check for previous registration
		user.setSendData(alreadyregistered(user));
		return;
	}

    // set user details
	user.setUsername(cmd.args[0]);
	user.setHostname(cmd.args[1]);
	user.setServername(cmd.args[2]);
	user.setFullname(cmd.args[3].substr(1));
	user.setStatus(USER_FLAG);
}

void Server::userMode(User &user, Command &cmd) {
	// extracting mode and arguments
	std::string mode = cmd.args[1];
	std::string changes;

	// dertemine mode operation
	bool on = mode.at(0) == '+' ? true : false;
	changes.append(1, mode.at(0));

	//store the flags
	std::set<char> flags;
	for(size_t i = 1; i < mode.size(); i++) {
		flags.insert(mode.at(i));
	}
	
	//apply changes to user
	std::set<char>::iterator it = flags.begin();
	for (; it != flags.end(); it++) {
		if (*it == 'o' && on) {
			it++;
			continue;
		}
		if (user.setMode(*it, on)) {
			changes.append(1, *it);
		}
	}

	//handle when no changes are made
	if (changes.length() == 1) return;

	// send mode changes to user
	return user.setSendData(":" + user.getNickname() + " MODE 221 " + changes + "\r\n");
}

void Server::oper(User &user, Command &cmd) {
	// logging the oper command
	logger.info("oper", "user " + user.getNickname() + " is trying to oper", logger.getLogTime());

	// check for sufficient arguments
	if (cmd.args.size() < 2) {
		user.setSendData(needmoreparams(user, "OPER"));
		return;
	} else if (cmd.args[1] != OPER_PASS) {
		user.setSendData(passworderror(user));
		return;
	} else if (cmd.args[0] != OPER_USER) {
		user.setSendData(nooper(user));
		return;
	}

    // granting operator mode
	if (user.setMode('o', true)) {
		user.setSendData(operOwnership(user));
	}
}

void Server::quit(User &user, Command &cmd) {
	// logging the quit command
	logger.info("quit", "user " + user.getNickname() + " is trying to quit", logger.getLogTime());

	//quit comand
	std::stringstream ss;
	ss << ":" << user.getNickname() << " QUIT\r\n";
	if(cmd.args.size()) {
		ss << ":" << user.getNickname() << " QUIT :" << cmd.args[0] << "\r\n";
	}
	else
	{
		ss << ":" << user.getNickname() << " QUIT: needed coffe\r\n";
	}

	// remove user from channels and broadcast quit message
	std::vector<Channel *>::iterator it = user.getChannelsJoined().begin();
	for (; it != user.getChannelsJoined().end(); it++) {
		(*it)->removeUser(user);
		(*it)->broadcast(user, ":" + user.getNickname() + " QUIT\r\n", false);
	}

	//logg quit command
	logger.debug("quit", "user " + user.getNickname() + " has quit", logger.getLogTime());

	//disconnect user
	user.setDisconnected(true);
}


bool Server::validChannelName(std::string Channelname) {
    // check for empty string
	if (Channelname.length() < 1)
        return false;

	std::string prefixes = CHANNEL_PREFIX;
    // check for valid prefix
	if (prefixes.find(Channelname.at(0)) == std::string::npos)
        return false;
    // check for excessive length
	if (Channelname.length() > 200)
        return false;

    // convert name to uppercase
	std::string insensitiveChannelname = toIrcUpperCase(Channelname);

    // check for forbiden characters
	std::string forbiddenChars	= " \a,:";
	for (std::size_t i = 0; i < forbiddenChars.size(); i++) {
		if (insensitiveChannelname.find(forbiddenChars.at(i)) != std::string::npos)
			return false;
	}

    // return true if all checks pass
	return true;
}

void Server::join(User &user, Command &cmd) 
{
	// check for sufficient arguments
	if (cmd.args.size() < 1) {
		user.setSendData(needmoreparams(user, "JOIN"));
		return;
	}

	// logging the join command
	logger.info("join", "user " + user.getNickname() + " is trying to join", logger.getLogTime());

	// validate channel name
	if (!validChannelName(cmd.args[0])) {
		user.setSendData(noChan(user, cmd.args[0]));
		return;
	}

	//password handling
	bool sentPassword = cmd.args.size() > 1;
	if(sentPassword)
	{
		bool v = validatePassword(cmd.args[1]);
		if(!v)
		{
			user.setSendData(needmoreparams(user, "JOIN"));
			return;
		}
	}

	//initialize channel
	Channel &ch = channels[toIrcUpperCase(cmd.args[0])];
	if (!ch.isInitialized()) {
		if (sentPassword) {
			ch.initializes(cmd.args[0], cmd.args[1], user);
		} else {
			ch.initialize(cmd.args[0], user);
		}
	}

	std::set<char> chModes = ch.getMode();
	//check user limit
	if (chModes.find('l') != chModes.end()) {
		if (ch.getUsers().size() >= ch.getUserLimit()) {
			user.setSendData(channelFull(user, ch.getChannelName()));
			return;
		}
	
	//invite only
	if (chModes.find('i') != chModes.end()) {
		if (!ch.isInvited(user.getNickname())) {
			user.setSendData(inviteOnlyChan(user, ch.getChannelName()));
			return;
		}
		ch.uninviteUser(user.getNickname());
		sucessfulJoin(user, ch);
	}

	//check password
	if (sentPassword)
	{
		if (!ch.checkKey(cmd.args[1]))
		{
			user.setSendData(wrongChannelKey(user, ch.getChannelName()));
			return;
		}
	}
	else 
	{
		if (!ch.checkKey(""))
		{
			user.setSendData(wrongChannelKey(user, ch.getChannelName()));
			return;
		}
	}

	//join channel
	sucessfulJoin(user, ch);
}
}

void Server::part(User &user, Command &cmd) {
	std::stringstream ss;

	// check for sufficient arguments
	if (cmd.args.size() < 1) {
		user.setSendData(needmoreparams(user, "PART"));
		return;
	}

	// logging the part command
	logger.info("part", "user " + user.getNickname() + " is trying to leave channel", logger.getLogTime());

	//check if user is in channel
	std::vector<Channel *> channels = user.getChannelsJoined();
	std::vector<Channel *>::iterator it = channels.begin();
	for (; it != channels.end(); it++) {
		if (toIrcUpperCase((*it)->getChannelName()) == toIrcUpperCase(cmd.args[0])) {
			if (cmd.args.size() != 2)
			{
				removeUserFromChannel(user, *(*it), ":" + user.getNickname());
				return;
			}
			else
			{
				removeUserFromChannel(user, *(*it), cmd.args[1]);
				return;
			}
		}
	}
	user.setSendData(noChan(user, cmd.args[0]));
	return;
}

void Server::invite(User &user, Command &cmd) {

	// check for sufficient arguments
	if (cmd.args.size() < 2) {
		user.setSendData(needmoreparams(user, "INVITE"));
		return;
	}
	// logging the invite command
	logger.info("invite", "user " + user.getNickname() + " is trying to invite", logger.getLogTime());
	//find channel
	std::map<std::string, Channel>::iterator it = getChannelName(cmd.args[1]);
	if (it == channels.end()) {
		user.setSendData(noNick(user, cmd.args[1]));
		return;
	}
	Channel &ch = it->second;
	//target user
	User *target = getUserByNickname(cmd.args[0]);
	if (target == NULL) {
		user.setSendData(noNick(user, cmd.args[0]));
		return;
	}
	//check if user is in channel
	std::map<User *, unsigned int>::iterator it2 = ch.getUserByNickname(user.getNickname());
	if (it2 == ch.getUsers().end()) {
		user.setSendData(notonchannel(user, ch.getChannelName()));
		return;
	}
	//check operator
	bool isOp = it2->second & CH_OPERATOR;

	//handle channel modes
	std::set<char> chModes = ch.getMode();
	if (chModes.find('i') != chModes.end() && !isOp) {
			user.setSendData(chanoprivsneeded(user, ch));
			return;
	}
	//add invitation and sent message
	ch.inviteUser(target->getNickname());
	user.setSendData(inviting(user, ch));
	target->setSendData(invitedReply(user, (*target), ch));
}

void Server::kick(User &user, Command &cmd) {
	// check for sufficient arguments
	if (cmd.args.size() < 2) {
		user.setSendData(needmoreparams(user, "KICK"));
		return;
	}
	// logging the kick command
	logger.info("kick", "user " + user.getNickname() + " is trying to kick", logger.getLogTime());
	//find channel
	Channel *ch = NULL;
	std::map<std::string, Channel>::iterator it = getChannelName(cmd.args[0]);
	if (it != channels.end()) {
		ch = it->second;
	}
	if (ch == NULL) {
		return user.setSendData(noChan(user, cmd.args[0]));
	}
	//target user
	std::map<User *, unsigned int>::iterator target;
	target = ch->getUserByNickname(cmd.args[1]);
	if (target == ch->getUsers().end()) {
		return user.setSendData(usernotinchannel(user, ch->getChannelName()));
	}
	//verify privileges
	std::map<User *, unsigned int>::iterator issuer;
	issuer = ch->getUserByNickname(user.getNickname());
	if (issuer == ch->getUsers().end()) {
		return user.setSendData(notonchannel(user, ch->getChannelName()));
	}
	if (!(issuer->second & USER_OPERATOR))
		return user.setSendData(chanoprivsneeded((*issuer->first), (*ch)));

    // handling the kick reason
	std::string targetName = target->first->getNickname();
	std::string reason = user.getNickname();
	if (cmd.args.size() > 2 && !cmd.args.at(2).empty()) {
		reason = cmd.args.at(2).find(':') == 0 ? cmd.args.at(2).substr(1) : cmd.args.at(2);
	}

    // broadcasting the kick and removing the user
	ch->broadcast(user, kicksuccess(user, *ch, targetName, reason), true);
	ch->removeUser(*target->first);
	return;
}

void Server::topic(User &user, Command &cmd) {
	// check for sufficient arguments
	if (cmd.args.size() < 1) {
		user.setSendData(needmoreparams(user, "TOPIC"));
		return;
	}
	// logging the topic command
	logger.info("topic", "user " + user.getNickname() + " is trying to change topic", logger.getLogTime());

	//retrieve the channel and manage topic query
	Channel* ch = channels[toIrcUpperCase(cmd.args[0])];

	std::stringstream ss;
	if (cmd.args.size() == 1) {
		if (ch->getTopic() != "") {
			ss.str("");
			ss << ":" << user.getNickname() << " TOPIC " << ch->getChannelName() << " :" << ch->getTopic() << "\r\n";
			return user.setSendData(ss.str());
		} else {
			return user.setSendData(":" + user.getNickname() + " 331 " + ch->getChannelName() + " :No topic is set\r\n");
		}
	}

    // check user's membership and permissions
	std::map<User *, uint> cls = ch->getUsers();
	std::map<User *, uint>::iterator it = cls.find(&user);
	std::set<char> md = ch->getMode();

	if (it == cls.end()) {
		return user.setSendData(notonchannel(user, ch->getChannelName()));
	}
	std::stringstream strings;
	strings << ":" << user.getNickname() << " TOPIC " << ch->getChannelName() << " :" << ch->getTopic() << "\r\n";

    // permission to change topic
	if (find(md.begin(), md.end(), 't') != md.end()) {
		std::map<User *, uint> cls = ch->getUsers();
		std::map<User *, uint>::iterator it = cls.find(&user);

		if (it != cls.end() && it->second & USER_OPERATOR) {
			ch->setTopic(cmd.args[1]);
			ch->broadcast(user, strings.str(), true);
		} else {
			user.setSendData(chanoprivsneeded(user, *ch));
		}
	} else {
		ch->setTopic(cmd.args[1]);
		ch->broadcast(user, strings.str(), true);
	}
}

void Server::kill(User &user, Command &cmd) {
	// logging the kill command
	logger.info("kill", "user " + user.getNickname() + " is trying to kill", logger.getLogTime());
	cmd.args.clear();
}

void Server::die(User &user, Command &cmd) {
	// logging the die command
	logger.info("die", "user " + user.getNickname() + " is trying to die", logger.getLogTime());
	cmd.args.clear();
}

void Server::privmsg(User &user, Command &cmd) {
	// logging the privmsg command
	logger.info("privmsg", "user " + user.getNickname() + " is trying to send message", logger.getLogTime());

	std::stringstream ss;
	std::string		  ch_prefix = CHANNEL_PREFIX;

    // check for sufficient arguments
	if (cmd.args.size() == 1) {
		if (ch_prefix.find(cmd.args[0].at(0)) != std::string::npos)
			return user.setSendData(norecipient(user, "PRIVMSG"));
		else
			return user.setSendData(notexttosend(user));
	}

	if (cmd.args.size() < 2) {
		user.setSendData(needmoreparams(user, "PRIVMSG"));
		return;
	}

    // constructing the message
	ss << user.getNickname();
	ss << " PRIVMSG";
	ss << " ";
	ss << cmd.args[0];
	ss << " ";
	ss << cmd.args[1];
	ss << "\r\n";

    // message dellivery to a channel
	if (ch_prefix.find(cmd.args[0].at(0)) != std::string::npos) {
		std::map<std::string, Channel*>::iterator it =
			channels.find(toIrcUpperCase(cmd.args[0]));

		if (it == channels.end())
			return user.setSendData(noNick(user, cmd.args[0]));
		else {
			Channel *ch = it->second;
			if (ch->getUsers().find(&user) == ch->getUsers().end())
				return user.setSendData(notonchannel(user, ch->getChannelName()));
			else
				return ch->broadcast(user, ss.str(), false);
		}
	} else { // message delivery to a user
		std::map<int, User>::iterator it = users.begin();

		for (; it != users.end(); it++) {
			if (it->second.getNickname() == cmd.args[0]) {
				return it->second.setSendData(ss.str());
			}
		}
		if (it == users.end()) {
			return user.setSendData(noNick(user, cmd.args[0]));
		}
	}

}