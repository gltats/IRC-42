//  This class is responsible for logging events that occur in the server.
//  It can log messages at different levels (e.g., debug, info, warning,
//  error) and can include the time of the event in the log message.
#pragma once

#include <iostream>
#include <string>
#include <ctime>


#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define GREEN   "\033[1;32m"

class Logger
{
    private:
        std::string logLevel;
        std::string logTime;

    public:
        Logger(); 
        Logger(std::string logLevel, std::string logTime);
        Logger(const Logger& logger);
        ~Logger();

        Logger &operator=(const Logger& logger);
        
        void debug(std::string method, std::string message, std::string logTime);
        void info(std::string method, std::string message, std::string logTime);
        void warning(std::string method, std::string message, std::string logTime);
        void error(std::string method, std::string message, std::string logTime);

        std::string getLogLevel();
        std::string getLogTime();

};