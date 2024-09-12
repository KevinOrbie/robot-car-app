/**
 * @brief C++ Socket POSIX socket interface.
 * @link https://www.linuxhowtos.org/C_C++/socket.htm
 */

/* ========================== Include ========================== */
#include "server.h"

/* Standard C Libraries */
// #include <poll.h>           // poll()
#include <stdio.h>          // IO Declarations
#include <fcntl.h>          // fcntl()
#include <string.h>         // bzero()
#include <stdlib.h>         // exit()
#include <unistd.h>         // read(), white()
#include <sys/types.h>      // Syscall datatypes
#include <sys/socket.h>     // Sockets support
#include <netinet/in.h>     // Internet domain address support (sockaddr_in)

/* Standard C++ Libraries */
// None


/* ========================= Functions ========================= */
static void error(char *msg) {
  perror(msg);
  exit(1);
}


namespace server {
/* ========================== Classes ========================== */

/* --------------------- Socket --------------------- */

Socket::Socket(int port, bool blocking): port_number(port), blocking(blocking) {
    sockaddr_in serv_addr = {};  // Socket internet address

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

    /* Fill in internet address. */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);  // Converted port to network byte order (Big Endian)
    serv_addr.sin_addr.s_addr = INADDR_ANY;   // Set IP to the server's host IP.

    /* Binds the opened socket to an address. */
    fprintf(stderr, "Binding Socket to address.\n");
    if (bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    /* Listen on the socket for connections. */
    // > Backlog Queue Size: 5 (the number of connections that can be waiting)
    fprintf(stderr,"Listening for client connections.\n");
    listen(socket_fd, 5);
};

Socket::~Socket() {
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

Connection Socket::link() {
    socklen_t clilen;  // Size of the address of the client
    sockaddr_in cli_addr = {};  // Client internet address

    /* Establish a connection with a single client. */
    clilen = sizeof(cli_addr);
    fprintf(stderr,"Waiting for a client connection...");

    /* Connect to socket. */
    int connection_fd = accept(socket_fd, (struct sockaddr *) &cli_addr, &clilen);  // Block until a client connects to the server
    if (connection_fd < 0) {
        error("ERROR on accept");
    }

    /* Construct connection. */
    fprintf(stderr, " >>> Client connected: %d\n", cli_addr.sin_addr.s_addr);
    Connection connection = Connection(connection_fd, blocking);

    /* We only need the established connection, delete the server socket. */
    close(socket_fd);
    socket_fd = -1;

    return connection;
};


/* --------------------- Server --------------------- */

Server::Server(int port, bool blocking): port_(port), blocking_(blocking_) {};

void Server::connect() {
    Socket socket = Socket(port_, blocking_);
    connection_ = socket.link(); // Wait for one client to connect.
}

} // namespace server

