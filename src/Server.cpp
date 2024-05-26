#include "Server.hpp"

Server::Server(int port, std::string password) : port(port), password(password), user(user), channel(channel)
{
    epollFd = epoll_create1(0);
    if (epollFd == -1)
    {
        throw std::runtime_error("Failed to create epoll instance\n");
    }
    this->port = port;
    this->password = password;
    this->socket = -1;
    this->maxConnectionsReached = false;
    // this->users = {};
    // this->channels = {};
    // this->connections = {};

    Logger logger;
    std::string logTime = logger.getLogTime();
    logger.info("Server", "Server created", logger.getLogTime());
}

Server::Server(const Server& server): user(server.user), channel(server.channel)
{
    port = server.port;
    password = server.password;
    socket = server.socket;
    users = server.users;
    channels = server.channels;
}

Server::~Server()
{
    if (!isServerSocketClosed) {
        close(socket);
    }
    logger.info("~Server", "Server destroyed", logger.getLogTime());
}

void Server::setupSocket()
{
    std::signal(SIGINT, handleSignal);

    logger.info("setupSocket", "Creating socket...", logger.getLogTime());
    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0)
    {
        throw std::runtime_error("Failed to create socket\n");
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    logger.info("setupSocket", "Binding socket...", logger.getLogTime());
    if (bind(socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        throw std::runtime_error("Failed to bind socket\n");
    }

    logger.info("setupSocket", "Listening on socket...", logger.getLogTime());
    if (listen(socket, 10) == -1)
    {
        throw std::runtime_error("Failed to listen on socket\n");
    }
}

Server* Server::instance = 0;
void Server::handleSignal(int signal)
{
    std::string logTime = Server::instance->logger.getLogTime();
    std::string logMessage = "HandleSignal called with signal " + std::to_string(signal);
    Server::instance->logger.info("Server", logMessage, logTime);
    if (signal == SIGINT)
    {
        Server::instance->stop();
    }
}

void Server::start()
{
    setupSocket();

    // initialize epoll
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        throw std::runtime_error("Failed to create epoll file descriptor\n");
    }

    struct epoll_event serverEvent;
    serverEvent.events = EPOLLIN;
    serverEvent.data.fd = socket;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, socket, &serverEvent) == -1) {
        logger.error("Epoll", "Failed to add server socket to epoll", logger.getLogTime());

    }

    // server startup logging
    logger.info("startServer", "Starting server...", logger.getLogTime());


    // main server loop
    while (true) {
        struct epoll_event events[MAX_EVENTS];
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1) {
            logger.error("Epoll", "Failed to wait for epoll events", logger.getLogTime());
        }

        for (int i = 0; i < numEvents; i++) {
            if (events[i].data.fd == socket) {
                // New connection on server socket
                handleServerEvents();
            } else {
                // Data available to read on a client socket
                handleUserEvents();
            }
        }

        handleDisconnectionEvents();
        handleEmptyChannelEvents();
    }
}

void Server::stop()
{
    logger.info("Server", "stop called", logger.getLogTime());
    if (!isServerSocketClosed) {
        close(socket);
        isServerSocketClosed = true;
    }
}

void Server::setPort(int port)
{
    if(port == 0)
    {
        throw std::runtime_error("Port is not set\n");
    }
    if(port <= 0 || port > 65535)
    {
        throw std::runtime_error("Invalid port number\n");
    }
    this->port = port;
}

void Server::setPassword(std::string password)
{
    if(password.empty() || password.find_first_of(FORBIDDEN_CHARS) != std::string::npos)
    {
        throw std::runtime_error("Invalid password\n");
    }
    this->password = password;
}


int Server::getPort()
{
    return port;
}

std::string Server::getPassword()
{
    return password;
}

int Server::getEpollFd() const
{
    return epollFd;
}

std::string Server::getHostname() const { return _hostname; }

User* Server::getUserByFd(int fd) {
    std::map<int, User>::iterator it = users.find(fd);
    if (it != users.end()) {
        return &(it->second);
    }
    return NULL;
}

std::vector<User*> Server::getAllUsers() {
    std::vector<User*> allUsers;
    for(auto& pair : users) {
        allUsers.push_back(&pair.second);
    }
    return allUsers;
}

User* Server::getUserByNickname(const std::string& nickname) {
    std::map<int, User>::iterator it;
    for(it = users.begin(); it != users.end(); ++it) {
        if(it->second.getNickname() == nickname) {
            return &(it->second);
        }
    }
    return NULL; // return null if no user with the given nickname is found
}