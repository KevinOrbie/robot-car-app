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


class Client {
   public:
    Client(std::string server_address, int port);

    /**
     * @brief Block until a Server connects to this client.
     */
    virtual void connect();
    virtual void iteration();
    virtual void thread();
    virtual void stop();

   protected:
    std::string server_address_ = "localhost";
    int port_                   = 2556;

    Connection connection_;
    std::unique_ptr<message::Reciever>    message_reciever_;
    std::unique_ptr<message::Transmitter> message_transmitter_;
};


} // namespace client