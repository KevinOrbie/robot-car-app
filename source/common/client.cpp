/**
 * @brief C++ Client POSIX socket interface.
 * @link https://www.linuxhowtos.org/C_C++/socket.htm
 */

/* ========================== Include ========================== */
#include "client.h"

/* Standard C Libraries */
#include <stdio.h>          // IO Declarations
#include <fcntl.h>          // fcntl()
#include <string.h>         // bzero()
#include <stdlib.h>         // exit()
#include <unistd.h>         // read(), white()
#include <sys/types.h>      // Syscall datatypes
#include <sys/socket.h>     // Sockets support
#include <netinet/in.h>     // Internet domain address support (sockaddr_in)
#include <netdb.h>          // hostent

/* Standard C++ Libraries */
#include <string>
#include <iostream>


/* ========================= Functions ========================= */
static void error(char *msg) {
  perror(msg);
  exit(1);
}


/* ========================== Classes ========================== */
ClientSocket::ClientSocket(std::string server_address, int port, bool blocking): blocking(blocking) {
    serv_addr = {};    // Address of the server to connect to
    hostent *server;   // Defines this host computer on the Internet

    /* Creates a new socket */
    fprintf(stderr, "Creating Socket.\n");
    socket_fd = socket(
        AF_INET,        // Address domain: AF_INET (internet domain).
        SOCK_STREAM,    // Socket Type: SOCK_STREAM (characters are read in a continuous stream).
        0               // Protocol: 0 (Choose appropiate protocol; TCP for stream socket).
    );
    if (socket_fd < 0) {
        error("ERROR opening socket");
    }

    /* Get hostent corresponding to hostname. */
    server = gethostbyname(server_address.c_str());
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host");
        exit(0);
    }

    /* Setup Server Address */
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
};

ClientSocket::~ClientSocket() {
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

Connection ClientSocket::link() {
    /* Establish a connection with the server. */
    fprintf(stderr, "Connecting to the Server.\n");
    if (connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    /* Construct connection. */
    Connection connection = Connection(socket_fd, blocking);

    /* Make sure the new destructor does not close connection. */
    socket_fd = -1;

    return connection;
};


// Client::Client(std::string server_address, int port, bool blocking) {
//     ClientSocket socket = ClientSocket(server_address, port, blocking);
//     connection_ = socket.link(); // Wait for server response.
// }

// void Client::send(const MessageData& message_data) {
//     Message msg = Message(&message_data);
//     msg.serialize(connection_);
// };

// std::unique_ptr<MessageData> Client::recieve() {
//     Message msg = Message();
//     return msg.deserialize(connection_);
// };


/* ========================= Entry-Point ========================= */
// int main(void) {
//     Client client = Client("localhost", 2556, false);
//     client.link();

//     /* Simulate control loop. */
//     while (true) {
//         std::string msg;
//         std::cin >> msg;
//         client.send(msg);
//     }
    
//     return 0;
// }
