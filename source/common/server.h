/**
 * @brief Header for C++ Server POSIX socket interface.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <string>
#include <memory>

/* Custom C++ Libraries */
#include "connection.h"


/* ========================== Classes ========================== */
class ServerSocket {
   public:
    ServerSocket(int port, bool blocking);
    ~ServerSocket();

    Connection link();

   private:
    int socket_fd      = -1;

    int port_number;
    bool blocking = false;
};

class Server {

};