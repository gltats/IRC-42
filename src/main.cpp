// File: main.cpp
#include "Server.hpp"
#include "Connection.hpp"
#include "Logger.hpp"

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
// #include "Server.hpp"
// #include "Logger.hpp"
// #include "Connection.hpp"
// int main()
// {
//     Server server(8080, "password");
//     server.start();

//     return 0;
// }

///the real main
int main(int argc, char *argv[])
{
    Logger logger;
    try
    {
        if (argc != 3)
        {
            throw std::invalid_argument("Usage: ./ircserv <port> <password>\n");
        }

        int port = std::stoi(argv[1]);
        std::string password = argv[2];

        
        Server server(port, password);
        server.start();
        
    } catch (std::exception &e)
    {
       logger.error("main", e.what(), logger.getLogTime());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

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

// #include "Channel.hpp"
// #include "User.hpp"
// #include <iostream>

// int main() {
//     // Create some users
//     User user1(1, "user1_hostname");
//     User user2(2, "user2_hostname");
    
//     // Set some user attributes
//     user1.setNickname("nick1");
//     user2.setNickname("nick2");
//     user1.setUsername("username1");
//     user2.setUsername("username2");
    
//     // Create some channels
//     Channel channel1("channel1", &user1);
//     Channel channel2("channel2", "key123", &user2);
    
//     // Add some users to channels
//     channel1.addUser(&user1);
//     channel1.addUser(&user2);
//     channel2.addUser(&user1);
    
//     // Test some getters
//     std::cout << "Channel 1 name: " << channel1.getChannelName() << std::endl;
//     std::cout << "Channel 2 key: " << channel2.getKey() << std::endl;
//     std::cout << "User 1 nickname: " << user1.getNickname() << std::endl;
//     std::cout << "User 2 hostname: " << user2.getHostname() << std::endl;
    
//     // Test adding and removing channels for users
//     user1.addChannelJoined("channel1");
//     user1.addChannelJoined("channel2");
//     user2.addChannelJoined("channel2");
//     user2.removeChannelJoined("channel1");
    
//     std::cout << "User 1 channels joined: ";
//     for (const auto &channel : user1.getChannelsJoined()) {
//         std::cout << channel << ", ";
//     }
//     std::cout << std::endl;
    
//     std::cout << "User 2 channels joined: ";
//     for (const auto &channel : user2.getChannelsJoined()) {
//         std::cout << channel << ", ";
//     }
//     std::cout << std::endl;
    
//     return 0;
// }
