/**
 * @brief Header for C++ Client POSIX socket interface.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <netinet/in.h>  // Internet domain address support (sockaddr_in)

/* Standard C++ Libraries */
#include <string>
#include <memory>

/* Custom C++ Libraries */
#include "connection.h"


/* ========================== Classes ========================== */
class ClientSocket {
   public:
    ClientSocket(std::string server_address, int port, bool blocking);
    ~ClientSocket();

    Connection link();

   private:
    int socket_fd;

    sockaddr_in serv_addr;
    bool blocking = false;
};

class Client {

};