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
#include <system_error>
#include <stdexcept>
#include <cstring>

#include <chrono>       // Time duration
#include <thread>       // Sleep thread

/* Custom C++ Libraries */
#include "common/logger.h"


namespace client {
/* ========================== Classes ========================== */

/* --------------------- Socket --------------------- */

Socket::Socket(std::string server_address, int port, bool blocking): blocking(blocking) {
    serv_addr = {};    // Address of the server to connect to
    hostent *server;   // Defines this host computer on the Internet

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

    /* Get hostent corresponding to hostname. */
    server = gethostbyname(server_address.c_str());
    if (server == NULL) {
        LOGE("%s", ("No such host '" + server_address + "'").c_str());
        throw std::runtime_error("No such host '" + server_address + "'");
    }

    /* Setup Server Address */
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
};

Socket::~Socket() {
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

Connection Socket::link() {
    /* Establish a connection with the server. */
    while (connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        if (errno == ECONNREFUSED) {
            /* No one listening on the remote address. */
            LOGW("Server not listening for connections, trying again in 5 seconds.");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }
        
        /* Error encountered while connecting. */
        LOGE("Error %d while connecting: %s", errno ,std::strerror(errno));
        throw std::system_error(errno, std::generic_category(), "Connecting");
    }

    /* Construct connection. */
    Connection connection = Connection(socket_fd, blocking);

    /* Make sure the new destructor does not close connection. */
    socket_fd = -1;

    return connection;
};


/* --------------------- Client --------------------- */

Client::Client(std::string server_address, int port, bool blocking): 
    server_address_(server_address), port_(port), blocking_(blocking) {};

void Client::connect() {
    Socket socket = Socket(server_address_, port_, blocking_);
    LOGI("Connecting to a server on: '%s'", server_address_.c_str());
    connection_ = socket.link(); // Wait for server response.
}

void Client::iteration() {
    send();
    // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    recieve();
};

} // namespace client
