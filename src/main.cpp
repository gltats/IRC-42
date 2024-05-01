// File: main.cpp
// #include "Server.hpp"


///logger test
// #include "Logger.hpp"

// int main() {
//     Logger logger;

//     std::string method = "main";
//     std::string message = "This is a test message";
//     std::string logTime = logger.getLogTime();

//     logger.debug(method, message, logTime);
//     logger.info(method, message, logTime);
//     logger.warning(method, message, logTime);
//     logger.error(method, message, logTime);

//     return 0;
// }

//tests for sockets
#include "Server.hpp"
#include "Logger.hpp"
#include "Connection.hpp"
int main()
{
    Server server(8080, "password");
    server.start();

    return 0;
}

///the real main
// int main(int argc, char *argv[])
// {
//     try
//     {
//         if (argc != 3)
//         {
//             throw std::invalid_argument("Usage: ./ircserv <port> <password>\n");
//         }

//         int port = std::stoi(argv[1]);
//         std::string password = argv[2];

//         Server server(port, password);
//         server.startServer();
//     } catch (std::exception &e)
//     {
//         std::cerr << e.what();
//         return EXIT_FAILURE;
//     }
//     return EXIT_SUCCESS;
// }

//STRUCTURE IDEA
// Server
//  - ServerSocket
//  - ClientSocket
//  - Server
//  - Client/User
//  - Channel
//  - Message
//  - Coonection
//  - Commands
//  - Parser?
//  - Logger
//     +-----------------+    1     +-----------------+
//     |     Server      |<-------->|    Connection   |
//     +-----------------+          +-----------------+
//     | -port: int      |          | -socket: int   |
//     | -password: str  |          | -clientSocket: int |
//     | -serverSocket: int        /| -send(message: str)
//     | -users: vector<User*>    / | -receive(): str
//     | -channels: vector<Channel*> 
//     | -connections: vector<Connection*>
//     +-----------------+ 
//     | -start()        | 
//     | -stop()         |
//     | -acceptConnection()
//     | -handleConnection(connection: Connection*)
//     +-----------------+ 
//             | 1
//             | 
//             V *
//     +-----------------+    *     +-----------------+
//     |      User       |<-------->|     Channel     |
//     +-----------------+          +-----------------+
//     | -nickname: str  |          | -name: str      |
//     | -username: str  |          | -topic: str     |
//     | -password: str  |          | -users: vector<User*> |
//     +-----------------+          | -addUser(user: User*)
//     | -getNickname(): str       | -removeUser(user: User*)
//     | -getUsername(): str       | -broadcastMessage(message: str)
//     | -authenticate(password: str): bool
//     +-----------------+          | -setTopic(topic: str): void
//                                  | -getTopic(): str
//                                  +-----------------+