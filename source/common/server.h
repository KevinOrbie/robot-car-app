/**
 * @brief Header for C++ Server POSIX socket interface.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <string>


/* ========================== Classes ========================== */
class Server {
   public:
    Server(int port, bool blocking);
    ~Server();

    void link();
    /**
     * @warning: This reads all recieved message, and outputs them on after another, in the same string.
     */
    std::string recieve();
    void send(std::string msg);

   private:
    int socket_fd      = -1;
    int connection_fd  = -1;

    int port_number;
    bool blocking = false;
};