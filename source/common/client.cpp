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
Client::Client(std::string server_address, int port, bool blocking): blocking(blocking) {
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

Client::~Client() {
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

void Client::link() {
    /* Establish a connection with the server. */
    fprintf(stderr, "Connecting to the Server.\n");
    if (connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    /* Make it socket non-blocking. */
    if (!blocking) {
        fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    }
};

std::string Client::recieve() {
    int chars_read = -1;
    std::string buffer = "";
    buffer.resize(255, 0);

    /* Recieve socket data (blocking wait). */
    if (!blocking && chars_read < 0 && ((errno & EAGAIN) || (errno & EWOULDBLOCK))){
        fprintf(stderr, "Nothing to read.\n");
        return std::string();

    } else if (chars_read < 0) {
        error("ERROR reading from socket");
    }

    return buffer;
};

void Client::send(std::string msg) {
    int chars_written = -1;

    /* Send socket data. */
    chars_written = write(socket_fd, msg.c_str(), msg.size()); 
    // NOTE: I don't know if we really need to check if non-blocking is ready here?
    if (chars_written < 0) error("ERROR writing to socket");

    return;
};


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