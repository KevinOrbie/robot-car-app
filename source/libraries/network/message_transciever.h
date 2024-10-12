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
#include <condition_variable>

/* Custom C++ Libraries */
#include "common/looper.h"
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


class Transmitter: public Looper {
   public:
    Transmitter(Connection* connection=nullptr): connection_(connection) {};

    void iteration() override;
    void setup() override;

    /**
     * @brief Send a single message.
     */
    void send(std::unique_ptr<message::MessageBase> msg=nullptr);
    void pushSendQueue(std::unique_ptr<message::MessageBase> msg);
    int getQueueSize();

   protected:
    void sendMessageID(MessageID id);
    std::unique_ptr<message::MessageBase> popSendQueue();

    /**
     * @brief Block this thread until a message is available.
     * @return True if a message is available, false on timeout.
     */
    bool waitForMessage(int timeout_ms);

   protected:
    std::deque<std::unique_ptr<message::MessageBase>> send_queue_;
    std::condition_variable send_queue_nonempty_cv_;
    std::mutex send_queue_mutex_;

    Connection* connection_;
};

class Reciever: public Looper {
   public:
    Reciever(Connection* connection=nullptr): connection_(connection) {};

    void iteration() override;
    void setup() override;

    /**
     * @brief Recieve a single message.
     * @return True if a message was recieved, false otherwise.
     */
    bool recieve();

    /**
     * @brief Block this thread until a message is available.
     * @return True if a message was recieved, false on timeout.
     */
    bool waitForMessage(int timeout_ms);

    std::unique_ptr<message::MessageBase> popRecieveQueue();
    int getQueueSize();

   protected:
    MessageID recieveMessageID();
    void pushRecieveQueue(std::unique_ptr<message::MessageBase> msg);

   protected:
    std::deque<std::unique_ptr<message::MessageBase>> recieved_queue_;
    std::condition_variable recieved_queue_nonempty_cv_;
    std::mutex recieved_queue_mutex_;

    Connection* connection_;
};

} // namespace message
