/**
 * @brief C++ Socket POSIX socket interface.
 * @link https://www.linuxhowtos.org/C_C++/socket.htm
 */

/* ========================== Include ========================== */
#include "message_transciever.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <utility>   // move()
#include <memory>
#include <mutex>

/* Custom C++ Libraries */
#include "message.h"


namespace message {
/* ========================== Classes ========================== */

void Transciever::send(std::unique_ptr<message::MessageBase> msg) {
    /* No message given? */
    if (!msg) {
        /* Pop message from send Queue. */
        msg = popSendQueue();

        /* Return early if there is no message to send. */
        if (!msg) { return; }
    }

    /* Preliminary Checks. */
    if (!connection_.valid()) {
        LOGW("Message not sent: Invalid Connection! (Could be because it is not yet initialized)");
        return;
    }

    /* Send Header. */
    sendMessageID(msg->getID());
    
    /* Send Payload. */
    msg->serialize(connection_);
};

void Transciever::recieve() {
    /* Preliminary Checks. */
    if (!connection_.valid()) {
        LOGW("Message not recieved: Invalid Connection! (Could be because it is not yet initialized)");
        return;
    }

    /* Recieve Header. */
    MessageID id = recieveMessageID();
    
    /* Ignore if nothing was recieved. */
    if (id != MessageID::EMPTY) { 
        /* Recieve Message. */
        std::unique_ptr<message::MessageBase> msg = MessageBase::deserialize(id, connection_);
        pushRecieveQueue(msg);
    }
};


void Transciever::sendMessageID(MessageID id) {
    LOGI("Sending MessageID: %d", static_cast<int>(id));
    int message_id = static_cast<int>(id);
    char* type_buffer = reinterpret_cast<char*>(&message_id);
    int num_bytes = sizeof(int);
    connection_.send(type_buffer, num_bytes);
}

MessageID Transciever::recieveMessageID() {
    int message_id = -1;

    /* Recieve the MessageID as an interger. */
    bool message_available = connection_.recieve(reinterpret_cast<char*>(&message_id), sizeof(int));

    /* Check if something was recieved. */
    if(!message_available) {
        return MessageID::EMPTY;
    };

    /* Find the corresponding MessageID. */
    MessageID id = static_cast<MessageID>(message_id);
    return id;
}

void Transciever::pushSendQueue(std::unique_ptr<message::MessageBase> msg) {
    if (!msg) { LOGW("Don't push nullpointers to the send queue."); return; }
    std::lock_guard lock(send_queue_mutex_);
    send_queue_.push_back(msg);
};

void Transciever::pushRecieveQueue(std::unique_ptr<message::MessageBase> msg) {
    if (!msg) { LOGW("Don't push nullpointers to the recieve queue."); return; }
    std::lock_guard lock(recieved_queue_mutex_);
    recieved_queue_.push_back(msg);
};

std::unique_ptr<message::MessageBase> Transciever::popSendQueue() {
    std::lock_guard lock(send_queue_mutex_);
    if (send_queue_.empty()) { return nullptr; }
    std::unique_ptr<message::MessageBase> msg = std::move(send_queue_.front());
    send_queue_.pop_front();
    return msg;
};

std::unique_ptr<message::MessageBase> Transciever::popRecieveQueue() {
    std::lock_guard lock(recieved_queue_mutex_);
    if (recieved_queue_.empty()) { return nullptr; }
    std::unique_ptr<message::MessageBase> msg = std::move(recieved_queue_.front());
    recieved_queue_.pop_front();
    return msg;
};


} // namespace server

