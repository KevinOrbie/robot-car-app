/**
 * @brief Header for C++ Client POSIX socket interface.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <netinet/in.h>  // Internet domain address support (sockaddr_in)

/* Standard C++ Libraries */
#include <string>


/* ========================== Classes ========================== */
class Client {
   public:
    Client(std::string server_address, int port, bool blocking);
    ~Client();

    void link();
    std::string recieve();
    void send(std::string msg);

   private:
    int socket_fd;

    sockaddr_in serv_addr;
    bool blocking = false;
};