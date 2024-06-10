//This class represents a user of the IRC server. 
// It's responsible for storing the user's nickname, username, and 
// password, and for authenticating the user.

#pragma once

#include "Logger.hpp"
// #include "Replies.hpp"
#include "Channel.hpp"
#include "Parser.hpp"
// #include "Server.hpp"

#include <string>
# include <iostream>
# include <vector>
# include <deque>
# include <ctime>

# define ST_DEAD 0
# define ST_ALIVE 1
# define ST_PINGED 3
# define ST_PONGED 5
# define ST_DISCONNECTED 7

// flags for user registration
#define NICK_FLAG 8
#define USER_FLAG 4
#define PASS_FLAG 2

#define OPER_USER "foo"
#define OPER_PASS "bar"

//   The available modes are:
//		none												=> 0	0000 0000
//		a - user is flagged as away;						=> 1	0000 0001
//		w - user receives wallops;							=> 2	0000 0010			
//     	i - marks a users as invisible; 					=> 3	0000 0100
//      o - operator flag;  								=> 4	0000 1000
//      B - marks a bot;									=> 5	0001 0000
//      s - marks a user for receipt of server notices.		=> 6	0010 0000
//      r - restricted user connection;	 					=> 7	0100 0000

#define MOD_NONE        (0 << 0)
// #define MOD_AWAY        (1 << 0)
// #define MOD_WALLOPS     (1 << 2)
// #define MOD_INVISIBLE   (1 << 3)
#define USER_OPERATOR       (1 << 4)
#define USER_INVITE		 (1 << 5)
#define USER_TOPIC		 (1 << 6)
#define USER_CHANNEL_KEY (1 << 7)

// #define MOD_BOT    		(1 << 5)
// #define MOD_SRVNOTICES  (1 << 6)
// #define MOD_RESTRICTED  (1 << 7)

class Channel;

class User {
	private:
		// Server&						_server;
		const int					_fd;
		std::string					_nickname;			// max 9 characters
		std::string					_username;			
		std::string 				_fullname;
		std::string					_hostname;
		std::string					_servername;
		std::string					sendData;
		bool						_password;
		bool						_authenticated;
		bool						_disconnected;
		bool						_welcome;
		std::vector<Channel *>		_channelsJoined;
		int							_status;
        time_t                      _lastActivityTime;
        time_t                      _pingTime;
		bool						_disconnect;
		unsigned int flags;


	public:
		User(const int fd, std::string hostname);
		User(const User &src);
		~User();

		User &operator=(User const &rhs);

		int							getFd(void) const;
		std::string 				getNickname(void) const;
		std::string 				getUsername(void) const;
		std::string 				getFullname(void) const;
		std::string 				getHostname(void) const;
		bool						getOperator(void) const;
		std::string					getModesStr();
		bool 						getPassword(void) const;
		bool 						getAuthenticated(void) const;
		bool 						getDisconnect(void) const;
		bool 						getWelcome() const;
		std::string					getSendData() const;
		std::vector<Channel *>		getChannelsJoined(void) const;
		int							getStatus(void) const;
        time_t  					getLastActivityTime(void) const;
        time_t  					getPingTime(void) const;
		bool 						getIsBot(void) const;
		void						removeChannel(Channel *ch);
		// Server& getServer();

		void setNickname(std::string nickname);
		void setUsername(std::string username);
		void setFullname(std::string fullname);
		void setHostname(std::string fullname);
		void setServername(std::string name);
		void setPassword(bool pass);
		void setAuthenticated(bool authenticated);
		void setWelcome(bool value);
		void setSendData(std::string data);
		void setStatus(int status);
		void setLastActivityTime(void);
		void setDisconnected(bool value);
		void setPingTime(void);
		void setIsBot(bool bot);
		void setOp(bool value);	
		bool setOperator(bool on);
		bool setInvite(bool on);
		bool setTopic(bool on);
		bool setChannelKey(bool on);
		bool setMode(char mode, bool on);


		//methods
		void resetSendData(int len);	//check
		
		void addChannel(Channel &channel);
		bool addChannelJoined(std::string channelName);			// if channel is already in the list, return false, else add + return true 
		bool removeChannelJoined(std::string channelName);		// if channel is found in the list, erase it + return true, else do nothing and return false
};

std::ostream & operator<<(std::ostream &o, User const &rhs);	// for printing 'nickname!username@host.name.com ...'

