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


class Server: public message::Transciever {
   public:
    Server(int port, bool blocking);

    /**
     * @brief Block until a client connects to the server.
     */
    void connect();

    void iteration();

   protected:

    /**
     * @brief Recieve message payload & take relevant actions.
     */
    template<message::MessageID ID> void handleMessage();

   private:
    int port_        = 2556;
    bool blocking_   = false;
};


/* ====================== Message Handlers ===================== */
/* NOTE: Declarations with template specialization is best done in the same header file. */

template<> void Server::handleMessage<message::MessageID::CMD_DRIVE>();


} // namespace server
