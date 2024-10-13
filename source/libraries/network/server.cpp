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
#include <system_error>
#include <stdexcept>
#include <cstring>

#include <chrono>       // Time duration
#include <thread>       // Sleep thread

/* Custom C++ Libraries */
#include "common/logger.h"


namespace server {
/* ========================== Classes ========================== */

/* --------------------- Socket --------------------- */

Socket::Socket(int port, bool blocking): port_number(port), blocking(blocking) {
    sockaddr_in serv_addr = {};  // Socket internet address

    /* Creates a new socket */
    LOGI("Creating Socket.");
    socket_fd = socket(
        AF_INET,        // Address domain: AF_INET (internet domain).
        SOCK_STREAM,    // Socket Type: SOCK_STREAM (characters are read in a continuous stream).
        0               // Protocol: 0 (Choose appropiate protocol; TCP for stream socket).
    );
    if (socket_fd < 0) {
        LOGE("Opening Socket: %s", std::strerror(errno));
        throw std::system_error(errno, std::generic_category(), "Opening Socket");
    }

    /* Fill in internet address. */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);  // Converted port to network byte order (Big Endian)
    serv_addr.sin_addr.s_addr = INADDR_ANY;   // Set IP to the server's host IP.

    /* Binds the opened socket to an address. */
    LOGI("Binding Socket to address.");
    if (bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        LOGE("Binding socket to server address: %s", std::strerror(errno));
        throw std::system_error(errno, std::generic_category(), "Binding socket to server address");
    }

    /* Listen on the socket for connections. */
    // > Backlog Queue Size: 5 (the number of connections that can be waiting)
    LOGI("Listening for client connections.");
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
    LOGI("Waiting for a client connection on port '%d'...", port_number);

    /* Connect to socket. */
    int connection_fd = accept(socket_fd, (struct sockaddr *) &cli_addr, &clilen);  // Block until a client connects to the server
    if (connection_fd < 0) {
        LOGE("Socket on accepting clients: %s", std::strerror(errno));
        throw std::system_error(errno, std::generic_category(), "Socket on accepting clients");
    }

    /* Construct connection. */
    LOGI("Connected to client: %d", cli_addr.sin_addr.s_addr);
    Connection connection = Connection(connection_fd, blocking);

    /* We only need the established connection, delete the server socket. */
    close(socket_fd);
    socket_fd = -1;

    return connection;
};


/* --------------------- Server --------------------- */
Server::Server(int port): port_(port){};

void Server::connect() {
    Socket socket = Socket(port_, false);
    connection_ = socket.link(); // Blocks, waiting for server response.

    /* Initalize Reciever & Transmitter. */
    message_transmitter_ = std::make_unique<message::Transmitter>(&connection_);
    message_reciever_ = std::make_unique<message::Reciever>(&connection_);
}

void Server::iteration() {
    message_transmitter_->iteration();
    message_reciever_->iteration();
};

void Server::thread() {
    if (!message_transmitter_ || !message_reciever_){
        LOGE("Transmitter or Reciever not yet initialized!");
        throw std::runtime_error("Transmitter or Reciever not yet initialized!");
    }

    message_transmitter_->thread();
    message_reciever_->thread();
};

void Server::stop() {
    LOGI("Stopping Reciever!");
    message_reciever_->stop();
    LOGI("Stopping Transmitter!");
    message_transmitter_->stop();
};

} // namespace server
