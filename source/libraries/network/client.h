/**
 * @brief Header for C++ Client POSIX socket interface.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <netinet/in.h>  // Internet domain address support (sockaddr_in)

/* Standard C++ Libraries */
#include <string>
#include <memory>

/* Custom C++ Libraries */
#include "message_transciever.h"
#include "connection.h"
#include "message.h"


namespace client {
/* ========================== Classes ========================== */
class Socket {
   public:
    Socket(std::string server_address, int port, bool blocking);
    ~Socket();

    Connection link();

   private:
    int socket_fd;

    sockaddr_in serv_addr;
    bool blocking = false;
};


class Client : public message::Transciever {
   public:
    Client(std::string server_address, int port, bool blocking);

    /**
     * @brief Block until a Server connects to this client.
     */
    void connect();

    void iteration();

   protected:

   private:
    std::string server_address_ = "localhost";
    int port_                   = 2556;
    bool blocking_              = false;
};


} // namespace client