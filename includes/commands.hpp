#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include <vector>
# include <string>
# include <iostream>
# include <sys/socket.h>
# include <typeinfo>

class Server;
class User;

// Connection commands
void	user(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);
void 	mode(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);

// Channel command
void    join(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);
void    part(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);
void    invite(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);
void    kick(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);
void    topic(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);
void    list(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);
void    names(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);


// Sending message commands
void    privmsg(const int &fd, const std::vector<std::string> &params, const std::string &prefix, Server *srv);


#endif
