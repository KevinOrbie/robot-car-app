/**
 * @brief Header for C++ POSIX socket interface.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
// None


/* ========================== Classes ========================== */
class Connection {
   public:
    Connection(int fd, bool blocking);

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
    bool recieve(char* buffer, int bytes);
    bool send(char* buffer, int bytes);

   private:
    int connection_fd_  = -1;
    bool blocking_ = false;
};

