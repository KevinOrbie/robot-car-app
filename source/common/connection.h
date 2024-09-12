/**
 * @brief Header for C++ POSIX socket interface.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
// None


/* ========================== Classes ========================== */
class Connection {
   public:
    /* Also acts as default constructor. */
    Connection(int fd=-1, bool blocking=false);

    /**
     * @note By only allowing move semantics, we make sure that the 
     * destructor is only called once, making sure the socket is not 
     * closed early.
     */

    /* Special member functions (Rule of 5). */
    Connection(const Connection& other) = delete;  /* Forbid Copying */
    Connection(Connection&& other);
    Connection& operator=(const Connection& other) = delete;  /* Forbid Copying */
    Connection& operator=(Connection&& other);
    ~Connection();

   public:
    bool recieve(char* buffer, int bytes) const;
    // bool recieve(int &value) const; (In case of memory mismatch)

    bool send(char* buffer, int bytes) const;
    // bool send(int value) const;

    bool valid() const { return connection_fd_ >= 0; };

   private:
    int connection_fd_  = -1;
    bool blocking_      = false;
};

