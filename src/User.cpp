#include "User.hpp"
#include <ctime>

// CONSTRUCTORS
User::User(const int fd, std::string hostname) 
    : _fd(fd),
	 _password(false),
	  _authenticated(false),
	   _welcome(false),
		 _status(0),
		 _lastActivityTime(time(NULL)),
		  _pingTime(time(NULL)),
		   _disconnect(false),
		   flags(USER_OPERATOR|USER_INVITE|USER_TOPIC|USER_CHANNEL_KEY)
{
	this->_hostname = hostname;
}

User::User(const User &src) 
    : _fd(src._fd),
	  _nickname(src._nickname),
	       _password(src._password),
	        _authenticated(src._authenticated),
	         _welcome(src._welcome),
				_status(src._status),
				_lastActivityTime(src._lastActivityTime),
	             _pingTime(src._pingTime),
	              _disconnect(src._disconnect),
	              flags(src.flags)
{
    *this = src;
}

// DESTRUCTORS
User::~User() { 
	for(size_t i = 0; i < _channelsJoined.size(); i++) {
		delete _channelsJoined[i];
	}
}

// ASSIGN OVERLOAD
User& User::operator=(User const &rhs) {
	if (this != &rhs) {
		this->_nickname = rhs._nickname;
		this->_username = rhs._username;
		this->_fullname = rhs._fullname;
		this->_hostname = rhs._hostname;
		this->_servername = rhs._servername;
		this->_password = rhs._password;
		this->_authenticated = rhs._authenticated;
		this->_welcome = rhs._welcome;
		this->_channelsJoined = rhs._channelsJoined;
		this->_status = rhs._status;
		this->flags = rhs.flags;
		this->_disconnected = rhs._disconnected;

	}
	return *this;
}

// GETTERS
int							User::getFd(void) const { return this->_fd; }
std::string					User::getNickname(void) const { return this->_nickname; }
std::string					User::getUsername(void) const { return this->_username; }
std::string					User::getFullname(void) const { return this->_fullname; }
std::string					User::getHostname(void) const { return this->_hostname; }
bool 						User::getPassword(void) const { return this->_password; }
bool 						User::getAuthenticated(void) const {
	return this->_authenticated;
}
bool 						User::getDisconnect(void) const { return this->_disconnect; }
bool						User::getWelcome() const { return _welcome; }
std::string					User::getSendData() const { return this->sendData; }
std::vector<Channel *>		User::getChannelsJoined(void) const {
	return _channelsJoined;
}

int							User::getStatus(void) const { return this->_status; }
time_t  					User::getLastActivityTime(void) const { return this->_lastActivityTime; }
time_t  					User::getPingTime(void) const { return this->_pingTime; }
bool 						User::getOperator(void) const { return flags & USER_OPERATOR; }

// SETTERS
void User::setNickname(std::string nickname) { this->_nickname = nickname; }
void User::setUsername(std::string username) { this->_username = username; }
void User::setFullname(std::string fullname) { this->_fullname = fullname; }
void User::setHostname(std::string hostname) { this->_hostname = hostname; }
void User::setServername(std::string name) { this->_servername = name; }
void User::setPassword(bool password) { this->_password = password; }
void User::setAuthenticated(bool authenticated) { 
	this->_authenticated = authenticated; 
}
void User::setWelcome(bool value) { this->_welcome = value; }
void User::setSendData(std::string data) { this->sendData = data; }
void User::setStatus(int status) { this->_status = status; }
void User::setLastActivityTime(void) { this->_lastActivityTime = time(NULL); }
void User::setDisconnected(bool disconnect) { this->_disconnect = disconnect; }
void User::setPingTime(void) { this->_pingTime = time(NULL); }

std::string User::getUserPrefix() {
	std::stringstream ss;

	ss << ":" << getNickname() << "!" << getUsername() << "@" << getHostname();
	return ss.str();
}

std::string User::getModesStr()
{
	std::string modes = "";
	if (USER_OPERATOR & flags)
		modes.append(1, 'o');
	if (USER_INVITE & flags)
		modes.append(1, 'i');
	if (USER_TOPIC & flags)
		modes.append(1, 't');
	if (USER_CHANNEL_KEY & flags)
		modes.append(1, 'k');
	return modes;
}

void User::setOp(bool value) {
	if (value)
		flags |= USER_OPERATOR;
	else
		flags = flags & ~USER_OPERATOR;
}

bool User::setOperator(bool on) {
	unsigned int before = flags;

	if (on) {
		flags |= USER_OPERATOR;
	} else {
		flags = flags & ~USER_OPERATOR;
	}
	if (flags == before) {
		return false;
	}
	return true;
};

bool User::setInvite(bool on) {
	unsigned int before = flags;

	if (on) {
		flags |= USER_INVITE;
	} else {
		flags = flags & ~USER_INVITE;
	}
	if (flags == before) {
		return false;
	}
	return true;
};

bool User::setTopic(bool on) {
	unsigned int before = flags;

	if (on) {
		flags |= USER_TOPIC;
	} else {
		flags = flags & ~USER_TOPIC;
	}
	if (flags == before) {
		return false;
	}
	return true;
};

bool User::setChannelKey(bool on) {
	unsigned int before = flags;

	if (on) {
		flags |= USER_CHANNEL_KEY;
	} else {
		flags = flags & ~USER_CHANNEL_KEY;
	}
	if (flags == before) {
		return false;
	}
	return true;
};

// Change the modes
bool User::setMode(char mode, bool on) {
	bool changed = false;
	switch (mode) {
		case 'i':
			changed = this->setInvite(on);
			break;
		case 't':
			changed = this->setTopic(on);
			break;
		case 'k':
			changed = this->setChannelKey(on);
			break;
		case 'o':
			changed = this->setOperator(on);
			break;
		default:
			break;
	}
	return changed;
}




// CHANNEL JOINED MANAGEMENT
// This used to add a channel name to the list of channels joined by the user
bool User::addChannelJoined(std::string channelName) {
    std::vector<Channel*>::iterator it;
    std::vector<Channel*>::iterator ite = this->_channelsJoined.end();

	for (it = this->_channelsJoined.begin(); it < ite; ++it) {
		if ((*it)->getChannelName() == channelName)
			return false;
	}
	this->_channelsJoined.push_back(new Channel(channelName, this));
	return true;
}

// This to remove
// bool User::removeChannelJoined(std::string channelName) {
//     std::vector<Channel*>::iterator it;
//     std::vector<Channel*>::iterator ite = this->_channelsJoined.end();

// 	for (it = this->_channelsJoined.begin(); it < ite; ++it) {
// 		if ((*it)->getChannelName() == channelName) {
// 			delete *it;
// 			this->_channelsJoined.erase(it);
// 			return true;
// 		}
// 	}
// 	return false;
// }

void User::removeChannel(Channel &channel) {
	_channelsJoined.erase(find(_channelsJoined.begin(), _channelsJoined.end(), &channel));
}

void User::resetSendData(int len) { sendData = sendData.substr(len); }

// OSTREAM 
std::ostream & operator<<(std::ostream &o, User const &rhs) {
	if (rhs.getAuthenticated() == true)
		o << rhs.getNickname() << "!" << rhs.getUsername();
	return o;
}

void User::addChannel(Channel &ch) { _channelsJoined.push_back(&ch); };