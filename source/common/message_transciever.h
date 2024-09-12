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
#include "connection.h"
#include "messages.h"


namespace message {
/* ========================== Classes ========================== */

class Transciever {
   public:
    /**
     * @brief Send a specifc message.
     */
    template<MessageID ID>
    void send(Message<ID> msg);

    /**
     * @brief Recieve a single message.
     */
    void recieve();

   protected:
    void sendMessageID(MessageID id);
    MessageID recieveMessageID();

    /**
     * @brief When a message is recieved, this function calls 
     * the appropriate Message Handler. 
     */
    virtual void pipeMessage(MessageID id) = 0;

   protected:
    Connection connection_;
};


} // namespace message
