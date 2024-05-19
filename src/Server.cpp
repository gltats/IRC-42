#include "Server.hpp"

Server::Server(int port, std::string password) : port(port), password(password), isServerSocketClosed(false)
{
    epollFd = epoll_create1(0);
    if (epollFd == -1)
    {
        throw std::runtime_error("Failed to create epoll instance\n");
    }
    this->port = port;
    this->password = password;
    this->serverSocket = -1;
    Server::instance = this;
    // this->users = {};
    // this->channels = {};
    // this->connections = {};

    Logger logger;
    std::string logTime = logger.getLogTime();
    logger.info("Server", "Server created", logger.getLogTime());
}

Server::Server(const Server& server)
{
    port = server.port;
    password = server.password;
    serverSocket = server.serverSocket;
    users = server.users;
    channels = server.channels;
}

Server::~Server()
{
    if (!isServerSocketClosed) {
        close(serverSocket);
    }
    logger.info("~Server", "Server destroyed", logger.getLogTime());
}

void Server::setupSocket()
{
    signal(SIGINT, Server::HandleSignal);

    logger.info("setupSocket", "Creating socket...", logger.getLogTime());
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        throw std::runtime_error("Failed to create socket\n");
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    logger.info("setupSocket", "Binding socket...", logger.getLogTime());
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        throw std::runtime_error("Failed to bind socket\n");
    }

    logger.info("setupSocket", "Listening on socket...", logger.getLogTime());
    if (listen(serverSocket, 10) == -1)
    {
        throw std::runtime_error("Failed to listen on socket\n");
    }
}

void Server::start()
{
    setupSocket();
    logger.info("startServer", "Starting server...", logger.getLogTime());

    struct epoll_event events[MAX_EVENTS];
    while (true)
    {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1) {
            throw std::runtime_error("Failed to wait for epoll events\n");
        }

        for (int i = 0; i < numEvents; i++)
        {
            epollFds.push_back(events[i]);
            if (events[i].data.fd == serverSocket)
            {
                // New connection on server socket
                acceptConnection();
            }
            else
            {
                // Data available to read on a client socket
                Connection* connection = findConnectionBySocket(events[i].data.fd);
                if (connection != NULL)
                {
                    handleConnection(connection);
                }
            }
        }
    }
}

void Server::stop()
{
    logger.info("Server", "stop called", logger.getLogTime());
    if (!isServerSocketClosed) {
        close(serverSocket);
        isServerSocketClosed = true;
    }
}

void Server::acceptConnection()
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket < 0)
    {
        logger.error("Server", "Failed to accept connection", logger.getLogTime());
        return;
    }

    // Make the new socket non-blocking
    int flags = fcntl(clientSocket, F_GETFL, 0);
    fcntl(clientSocket, F_SETFL, flags | SOCK_NONBLOCK );

    // Add the new socket to the epoll instance
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Edge-triggered
    event.data.fd = clientSocket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1) {
        throw std::runtime_error("Failed to add client socket to epoll\n");
    }

    Connection* connection = new Connection(clientSocket, *this);
    connections.push_back(connection);
}

void Server::handleConnection(Connection* connection)
{
    std::string message = connection->receive();
    if (!message.empty()) {
        std::cout << message << std::endl;
        connection->send_data("Hello from server\n");
    } else {
        logger.info("Server", "Connection closed by client", logger.getLogTime());
        // Remove the connection from the connections vector and delete the Connection object
    }
}

Server* Server::instance = nullptr;
void Server::HandleSignal(int signal) //for control + c, control + z, etc
{
    std::string logTime = Server::instance->logger.getLogTime();
    std::string logMessage = "HandleSignal called with signal " + std::to_string(signal);
    Server::instance->logger.info("Server", logMessage, logTime);
    if (signal == SIGINT)
    {
        Server::instance->stop();
    }
}

Connection* Server::findConnectionBySocket(int socket)
{
    for (std::vector<Connection*>::iterator it = connections.begin(); it != connections.end(); ++it)
    {
        if ((*it)->getSocket() == socket)
        {
            return *it;
        }
    }
    return NULL;
}

int Server::getPort()
{
    return port;
}

std::string Server::getPassword()
{
    return password;
}


int Server::getServerSocket()
{
    return serverSocket;
}

std::vector<User*> Server::getUsers()
{
    return users;
}

std::vector<Channel*> Server::getChannels()
{
    return channels;
}

std::vector<Connection*> Server::getConnections()
{
    return connections;
}

std::vector<struct epoll_event>& Server::getEpollFds()
{
    return epollFds;
}

void Server::setServerSocket(int serverSocket)
{
    this->serverSocket = serverSocket;
}

int Server::getEpollFd()
{
    return epollFd;
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


void Server::setUsers(std::vector<User*> users)
{
    this->users = users;
}

void Server::setChannels(std::vector<Channel*> channels)
{
    this->channels = channels;
}

void Server::setConnections(std::vector<Connection*> connections)
{
    this->connections = connections;
}

