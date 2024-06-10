#include "Channel.hpp"

// constructor
Channel::Channel(void)
{
	host		= "localhost";
	creator		= NULL;
	userLimit	= USER_CHANNEL_LIMIT;
	initialized = false;
}

Channel::Channel(std::string name, User *creatorParam)
{
    logger.info("Channel", "Creating channel " + name, logger.getLogTime());
    host		= "localhost";
    creator		= creatorParam;
    userLimit	= USER_CHANNEL_LIMIT;
    initialized = false;
}

// destructor
Channel::~Channel(void)
{
    logger.info("~Channel", "Deleting channel ", logger.getLogTime());
    _users.clear();
    invited.clear();
}

Channel &Channel::operator=(Channel const &cpy)
{
    logger.info("operator=", "Copying channel " + cpy._channelName, logger.getLogTime());
    this->_channelName = cpy._channelName;
    this->_topic = cpy._topic;
    this->host = cpy.host;
    this->modes = cpy.modes;
    this->userLimit = cpy.userLimit;
    this->_users = cpy._users;
    this->initialized = cpy.initialized;
    this->creator = cpy.creator;
    this->invited = cpy.invited;
    return (*this);
}


/** Getters **/

std::string Channel::getChannelName(void) const { return (this->_channelName); }
std::string Channel::getTopic(void) const { return (this->_topic); }
User *Channel::getCreator(void) const { return (this->creator); }

std::set<char> &Channel::getMode() { return modes; }
unsigned int Channel::getUserLimit() const { return userLimit; }
std::map<User*, unsigned int>  &Channel::getUsers(void) { return (this->_users); }

std::string Channel::getStrModes() {
	std::string modeStr = "";

	if (modes.size() > 0) {
		modeStr += "+";
		std::set<char>::iterator it = modes.begin();
		while (it != modes.end()) {
			modeStr += (*it);
			it++;
		}
		if (modeStr.find('k') != std::string::npos) {
			modeStr.append(" ");
			modeStr.append(this->_key);
		}
	}
	return (modeStr);
}

std::map<User *, unsigned int>::iterator Channel::getUserByNickname(std::string nickname)
{
    logger.info("getUserByNickname", "Checking for user " + nickname, logger.getLogTime());
    std::map<User *, unsigned int>::iterator it = this->_users.begin();
    for (; it != this->_users.end(); it++)
    {
        if (it->first->getNickname() == nickname)
        {
            return (it);
        }
    }
    return (it);
}



/** Setters **/

void Channel::setTopic(std::string topic) { this->_topic = topic; }

void Channel::setKey(std::string key) { this->_key = key; }

void Channel::setUserLimit(unsigned int limit) { this->userLimit = limit; }

bool Channel::setOperator(std::string nickname, bool newValue)
{
    logger.info("setOperator", "Setting operator status for " + nickname, logger.getLogTime());
    std::map<User *, unsigned int>::iterator it = this->getUserByNickname(nickname);
    if (it != this->_users.end())
    {
        if (newValue)
        {
            it->second |= CH_OPERATOR;
        }
        else
        {
            it->second &= (~CH_OPERATOR);
        }
    }
    return (false);

}

void Channel::setSpeaker(std::string nickname, bool newValue)
{
    logger.info("setSpeaker", "Setting speaker status for " + nickname, logger.getLogTime());
    std::map<User *, unsigned int>::iterator it = this->getUserByNickname(nickname);
    if (it != this->_users.end())
    {
        if (newValue)
        {
            it->second |= CH_SPEAKER;
        }
        else
        {
            it->second &= ~CH_SPEAKER;
        }
    }
}

bool Channel::isInitialized() { return initialized; }

void Channel::addUser(User &user)
{
    _users.insert(std::make_pair(&user, 0));
}

void Channel::removeKey(void) {
    modes.erase('k');
    _key = "";
}

void Channel::removeUser(User &user)
{
    logger.info("removeUser", "Removing user " + user.getNickname() + " from channel " + _channelName, logger.getLogTime());
    if(creator == &user) {
        logger.info("removeUser", user.getNickname() + " is the Creator " + _channelName, logger.getLogTime());
        creator = NULL;
    }
    if(_users.size() > 0) {
        _users.erase(&user);
        makeOperator();
    }
}

void Channel::broadcast(User &sender, std::string message, bool toSend)
{
    std::map<User *, unsigned int>::iterator it = this->_users.begin();
    logger.info("broadcast", "Broadcasting message to all clients in channel " + _channelName, logger.getLogTime());
    for (; it != this->_users.end(); it++)
    {
        if (it->first == &sender && toSend) {
			(*it->first).setSendData(message);
		} else if (it->first != &sender) { // sender excluded
			(*it->first).setSendData(message);
		}
    }
}

bool Channel::manageMode(char mode, bool on)
{
    std::size_t before = modes.size();
    logger.info("manageMode", "Managing mode " + std::string(1, mode), logger.getLogTime());

    if (on) {
        modes.insert(mode);
    } else {
        modes.erase(mode);
    }

    return (before != modes.size());
}


void Channel::initializes(std::string channelName, std::string key, User &op)
{
    logger.info("initialize", "Initializing channel " + channelName, logger.getLogTime());
    this->_channelName = channelName;
    this->_key = key;
    this->_users.insert(std::make_pair(&op, CH_OPERATOR));
    this->initialized = true;
    this->modes.insert('t');
    this->creator = &op;
}

void Channel::initialize(std::string channelName, User &op)
{
    logger.info("initialize", "Initializing channel " + channelName, logger.getLogTime());
    this->_channelName = channelName;
    this->_users.insert(std::make_pair(&op, CH_OPERATOR));
    this->initialized = true;
    this->modes.insert('t');
    this->creator = &op;
    if (this->creator != &op) {
        logger.error("initialize", "Creator is not the operator", logger.getLogTime());
    }
    else
    {
        logger.info("initialize", "Creator is the operator", logger.getLogTime());
    }
}

bool Channel::checkKey(std::string key)
{ 
    logger.info("checkkey", "Checking key for channel " + _channelName, logger.getLogTime());
    return (this->_key == key);
}

bool Channel::isInvited(std::string nickname)
{
    if(invited.find(toIrcUpperCase(nickname)) != invited.end())
    {
        return true;
    }
    return false;
}

void Channel::inviteUser(std::string nickname)
{
    invited.insert(toIrcUpperCase(nickname));
}

void Channel::uninviteUser(std::string nickname)
{
    invited.erase(toIrcUpperCase(nickname));
}

void Channel::makeOperator()
{
    logger.info("makeOperator", "Checking if a new operator is needed in " + _channelName, logger.getLogTime());

    //existing operator
    std::map<User *, unsigned int>::iterator it = this->_users.begin();
    for (; it != this->_users.end(); it++)
    {
        if (it->second & CH_OPERATOR)
        {
            return;
        }
    }

    //assign new operator
    it = this->_users.begin();
    if (it != this->_users.end())
    {
        this->setOperator(it->first->getNickname(), true);
        logger.info("makeOperator", "New operator is " + it->first->getNickname(), logger.getLogTime());

        //notify new operator
        std::stringstream ss;
        ss << ":" << it->first->getNickname() << " MODE " << this->getChannelName() << " +o " << it->first->getNickname() << "\r\n";
        this->broadcast(*it->first, ss.str(), true);
    }
}