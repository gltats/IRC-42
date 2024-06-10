#include "Server.hpp"

void replaceString(std::string& subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

std::string toIrcUpperCase(std::string s) {
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	replaceString(s, "{", "[");
	replaceString(s, "}", "]");
	replaceString(s, "|", "\\");

	return s;
}


void Server::trim(std::string& s) {
	std::size_t start = 0;
	std::size_t end	  = s.length();

	while (start < end && s[start] == ' ') {
		start++;
	}
	s = s.substr(start);
}

void Server::trimAll(std::string& s) {
	std::size_t start = 0;
	std::size_t end	  = s.length();

	while (start < end && s[start] == ' ') {
		start++;
	}

	while (end > start && s[end - 1] == ' ') {
		end--;
	}
	s = s.substr(start, end - start);
}

void Server::cleanEndOfTransmission(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '\4'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
}

Command Server::messageToCommand(std::string source) {
	Command c;
	std::string token;

    // clean end transmission
	cleanEndOfTransmission(source);

    // check and extract prefix
	if (source[0] == ':') {
		std::istringstream sourceStream(source);
		std::getline(sourceStream, c.prefix, ' ');
		source = source.substr(source.find(c.prefix) + c.prefix.size());
	}

    // trim leading spaces
	trim(source);

    // extract command
	std::istringstream sourceStream(source);
	std::getline(sourceStream, c.cmd, ' ');
	source = source.substr(source.find(c.cmd) + c.cmd.size());
    // trim leading spaces
	trim(source);

    // extract arguments
	size_t colonPos = source.find(':');
	if (colonPos != std::string::npos) {
		std::string beforeColon = source.substr(0, colonPos);
		std::string afterColon	= source.substr(colonPos);

		std::istringstream beforeColonSteam(beforeColon);

		while (std::getline(beforeColonSteam, token, ' ')) {
			trimAll(token);
			if (token.size() != 0)
                c.args.push_back(token);
		}
		c.args.push_back(afterColon);
	} else {
		std::istringstream sourceStream(source);

		while (std::getline(sourceStream, token, ' ')) {
			trimAll(token);
			if (token.size() != 0)
                c.args.push_back(token);
		}
	}

    // return command
	return c;
}


std::vector<Command> Server::parseCommands(std::string data) {
	std::vector<Command> commands;
	size_t pos;

    // logging the parsing activity
	logger.info("parseCommands", "Parsing commands from data: " + data, logger.getLogTime());

    // command extraction loop
  if (data.find("\r\n") != std::string::npos) {
	 while (data.find("\r\n") != std::string::npos) {
		 pos = data.find("\r\n");
		 if (pos > 0) {
		  	commands.push_back(messageToCommand(data.substr(0, pos)));
			 data.erase(0, pos + 2);
		 }
	 }
  }
  else {
    commands.push_back(messageToCommand(data));
  }

    // return parsed commands
	return commands;
}

bool validatePassword(std::string password) {
	if (password.find(',') != std::string::npos) return false;
	return true;
}