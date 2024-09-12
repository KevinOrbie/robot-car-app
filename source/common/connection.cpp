/**
 * @brief C++ POSIX socket connection interface.
 * @link https://www.linuxhowtos.org/C_C++/socket.htm
 */

/* ========================== Include ========================== */
#include "connection.h"

/* Standard C Libraries */
// #include <poll.h>           // poll()
#include <stdio.h>          // IO Declarations
#include <errno.h>          // errno, ...
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

/* Custom C++ Includes */
#include "common/logger.h"


/* ========================== Classes ========================== */
Connection::Connection(int fd, bool blocking): connection_fd_(fd), blocking_(blocking) {
    /* Make it socket non-blocking. */
    if (!blocking) {
        fcntl(connection_fd_, F_SETFL, O_NONBLOCK);
    }
}

Connection::~Connection() {
    if (connection_fd_ >= 0) {
        close(connection_fd_);
    }
}

/* Move Constructor. */
Connection::Connection(Connection&& other) {
    connection_fd_ = other.connection_fd_;
    blocking_ = other.blocking_;

    /* Invalidate other Object. */
    other.connection_fd_ = -1;  // Prevents correct file from closing.
}

/* Move Assignment Operator. */
Connection& Connection::operator=(Connection&& other) {
    if (this != &other) {  /* Make sure not called on itself. */
        connection_fd_ = other.connection_fd_;
        blocking_ = other.blocking_;

        /* Invalidate other Object. */
        other.connection_fd_ = -1;
    }
    return *this;
}

/* ------------------------------------- Reception ------------------------------------ */
bool Connection::recieve(char* buffer, int bytes) const {
    int chars_read = -1;

    /* Read socket data to the given buffer. */
    chars_read = read(connection_fd_, buffer, bytes);

    /* Recieve socket data (blocking wait). */
    if (!blocking_ && chars_read < 0 && ((errno & EAGAIN) || (errno & EWOULDBLOCK))){
        return false; /* Nothing read. */

    } else if (chars_read < 0) {
        LOGE("Reading from socket: %s", std::strerror(errno));
        throw std::system_error(errno, std::generic_category(), "Reading from socket");
    }

    return true;
};

/* ----------------------------------- Transmission ----------------------------------- */
bool Connection::send(char* buffer, int bytes) const {
    LOGI("Sending Bytes: %d bytes, buffer @ %p ", bytes, buffer);
    int chars_written = -1;

    /* Send socket data. */
    chars_written = write(connection_fd_, buffer, bytes); 
    
    // NOTE: I don't know if we really need to check if non-blocking is ready here?
    if (chars_written < 0) {
        LOGE("Writing to socket: %s", std::strerror(errno));
        throw std::system_error(errno, std::generic_category(), "Writing to socket");
    }

    return true;
};
