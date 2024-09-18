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
#include <deque>
#include <mutex>

/* Custom C++ Libraries */
#include "connection.h"
#include "messages.h"


namespace message {
/* ========================== Classes ========================== */

class Transciever {
   public:
    /**
     * @brief Send a single message.
     */
    void send(std::unique_ptr<message::MessageBase> msg=nullptr);

    /**
     * @brief Recieve a single message.
     */
    void recieve();

    void pushSendQueue(std::unique_ptr<message::MessageBase> msg);
    std::unique_ptr<message::MessageBase> popRecieveQueue();

   protected:
    void sendMessageID(MessageID id);
    MessageID recieveMessageID();

    std::unique_ptr<message::MessageBase> popSendQueue();
    void pushRecieveQueue(std::unique_ptr<message::MessageBase> msg);

   protected:
    std::deque<std::unique_ptr<message::MessageBase>> send_queue_;
    std::mutex send_queue_mutex_;
    std::deque<std::unique_ptr<message::MessageBase>> recieved_queue_;
    std::mutex recieved_queue_mutex_;

    Connection connection_;
};


} // namespace message
