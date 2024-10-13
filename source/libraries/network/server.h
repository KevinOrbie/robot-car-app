/**
 * @brief Header for C++ Server POSIX socket interface.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <string>
#include <memory>

/* Custom C++ Libraries */
#include "message_transciever.h"
#include "connection.h"
#include "messages.h"


namespace server {
/* ========================== Classes ========================== */
class Socket {
   public:
    Socket(int port, bool blocking);
    ~Socket();

    Connection link();

   private:
    int socket_fd      = -1;

    int port_number;
    bool blocking = false;
};


class Server {
   public:
    Server(int port);

    /**
     * @brief Block until a client connects to the server.
     */
    virtual void connect();
    
    /* Looper Interface. */
    virtual void iteration();
    virtual void thread();
    virtual void stop();

   protected:
    int port_        = 2556;
    
    Connection connection_;
    std::unique_ptr<message::Reciever>    message_reciever_;
    std::unique_ptr<message::Transmitter> message_transmitter_;
};

} // namespace server
