#include "Logger.hpp"

Logger::Logger()
{
    logLevel = "INFO";
    logTime = "TIME";
}

Logger::Logger(std::string logLevel, std::string logTime)
{
    this->logLevel = logLevel;
    this->logTime = logTime;
}

Logger::Logger(const Logger& logger)
{
    logLevel = logger.logLevel;
    logTime = logger.logTime;
}

Logger::~Logger()
{
}

Logger &Logger::operator=(const Logger& logger)
{
    logLevel = logger.logLevel;
    logTime = logger.logTime;
    return *this;
}

void Logger::debug(std::string method, std::string message, std::string logTime)
{
    std::cout << "[" << logTime << "] " << BLUE << "[DEBUG] " << RESET << method << ": " << message << std::endl;
}


void Logger::info(std::string method, std::string message, std::string logTime)
{
    std::cout << "[" << logTime << "] " << GREEN << "[INFO] " << RESET << method << ": " << message << std::endl;
}

void Logger::warning(std::string method, std::string message, std::string logTime)
{
    std::cout << "[" << logTime << "] " << YELLOW << "[WARNING] " << RESET << method << ": " << message << std::endl;
}


void Logger::error(std::string method, std::string message, std::string logTime)
{
    std::cout << "[" << logTime << "] " << RED << "[ERROR] " << RESET << method << ": " << message << std::endl;
}

std::string Logger::getLogLevel()
{
    return logLevel;
}

std::string Logger::getLogTime()
{
    std::time_t currentTime;
    std::time(&currentTime);
    std::string logTime = std::ctime(&currentTime);
    logTime.erase(logTime.length() - 1); // Remove the newline character at the end

    return logTime;
}