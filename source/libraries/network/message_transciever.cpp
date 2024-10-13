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
#include <chrono>
#include <memory>
#include <mutex>

/* Custom C++ Libraries */
#include "common/logger.h"
#include "common/utils.h"  // gettid()
#include "message.h"


namespace message {
/* ============================== Transmitter ============================== */
void Transmitter::iteration() {
    /* If ran in seperate thread, make this thread block, while waiting for available message. */
    if (threaded()) {
        bool message_available = waitForMessage(1000);

        /* Wait again on timeout, needed to stop looper thread if requested. */
        if (!message_available) { return; }
    }

    do { /* Send all queued messages. */
        send();
    } while (getQueueSize() > 0);
}

void Transmitter::setup() {
    LOGI("Running Message Transmitter (TID = %d)", gettid());
};

void Transmitter::send(std::unique_ptr<message::MessageBase> msg) {
    LOGI("Transmitting Message!");

    /* No message given? */
    if (!msg) {
        /* Pop message from send Queue. */
        msg = popSendQueue();

        /* Return early if there is no message to send. */
        if (!msg) { return; }
    }

    /* Preliminary Checks. */
    if (!connection_ || !connection_->valid()) {
        LOGW("Message not sent: Invalid Connection! (Could be because it is not yet initialized)");
        return;
    }

    /* Send Header. */
    sendMessageID(msg->getID());
    
    /* Send Payload. */
    msg->serialize(*connection_);
};

void Transmitter::sendMessageID(MessageID id) {
    LOGI("Sending MessageID: %d", static_cast<int>(id));
    int message_id = static_cast<int>(id);
    char* type_buffer = reinterpret_cast<char*>(&message_id);
    int num_bytes = sizeof(int);
    connection_->send(type_buffer, num_bytes);
}

bool Transmitter::waitForMessage(int timeout_ms) {
    std::unique_lock<std::mutex> lock(send_queue_mutex_);
    return send_queue_nonempty_cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms)) == std::cv_status::no_timeout;
}

void Transmitter::pushSendQueue(std::unique_ptr<message::MessageBase> msg) {
    if (!msg) { 
        LOGW("Don't push nullpointers to the send queue."); 
        return; 
    }

    { /* Limit lock scope. */
        std::lock_guard lock(send_queue_mutex_);
        send_queue_.push_back(std::move(msg));
    }

    send_queue_nonempty_cv_.notify_all();
};

std::unique_ptr<message::MessageBase> Transmitter::popSendQueue() {
    std::lock_guard lock(send_queue_mutex_);
    if (send_queue_.empty()) { return nullptr; }
    std::unique_ptr<message::MessageBase> msg = std::move(send_queue_.front());
    send_queue_.pop_front();
    return msg;
};

int Transmitter::getQueueSize() {
    std::lock_guard lock(send_queue_mutex_);
    return send_queue_.size();
}


/* =============================== Reciever ================================ */
void Reciever::iteration() {
    /* If ran in seperate thread, make this thread block, while waiting to recieve a message. */
    if (threaded()) {
        bool message_available = connection_->wait(1000);
        
        /* Wait again on timeout, needed to stop looper thread if requested. */
        if (!message_available) { return; }
    }
    
    /* Recieve initial message. */
    bool new_message_recieved;
    new_message_recieved = recieve();

    /* Detect broken connection. */
    if (!new_message_recieved) {
        /* Nothing recieved, while poll indicated somehting was recieved. */
        LOGW("Connection broken, trying again in 3 seconds!");
        std::this_thread::sleep_for(std::chrono::seconds(3));  // Prevent busy polling.
        // TODO: possibly reset connection, and wait for connection.
    }
    
    /* Recieve all remaining messages in the kernel buffers. */
    while (new_message_recieved) {
        new_message_recieved = recieve(); /* Keep recieving one message at a time, until they are empty. */
    }
}

void Reciever::setup() {
    LOGI("Running Message Reciever (TID = %d)", gettid());
};

bool Reciever::recieve() {
    /* Preliminary Checks. */
    if (!connection_ || !connection_->valid()) {
        LOGW("Message not recieved: Invalid Connection! (Could be because it is not yet initialized)");
        return false;
    }

    /* Recieve Header. */
    MessageID id = recieveMessageID();
    
    /* Ignore if nothing was recieved. */
    if (id != MessageID::EMPTY) { 
        /* Recieve Message. */
        std::unique_ptr<message::MessageBase> msg = MessageBase::deserialize(id, *connection_);
        pushRecieveQueue(std::move(msg));
        return true;
    } else {
        return false;
    }
};

MessageID Reciever::recieveMessageID() {
    int message_id = -1;

    /* Recieve the MessageID as an interger. */
    bool message_available = connection_->recieve(reinterpret_cast<char*>(&message_id), sizeof(int));

    /* Check if something was recieved. */
    if(!message_available) {
        return MessageID::EMPTY;
    };

    /* Find the corresponding MessageID. */
    MessageID id = static_cast<MessageID>(message_id);
    return id;
}

bool Reciever::waitForMessage(int timeout_ms) {
    std::unique_lock<std::mutex> lock(recieved_queue_mutex_);
    return recieved_queue_nonempty_cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms)) == std::cv_status::no_timeout;
}

void Reciever::pushRecieveQueue(std::unique_ptr<message::MessageBase> msg) {
    if (!msg) { 
        LOGW("Don't push nullpointers to the recieve queue."); 
        return; 
    }

    { /* Limit lock scope. */
        std::lock_guard lock(recieved_queue_mutex_);
        recieved_queue_.push_back(std::move(msg));
    }

    recieved_queue_nonempty_cv_.notify_all();
};

std::unique_ptr<message::MessageBase> Reciever::popRecieveQueue() {
    std::lock_guard lock(recieved_queue_mutex_);
    if (recieved_queue_.empty()) { return nullptr; }
    std::unique_ptr<message::MessageBase> msg = std::move(recieved_queue_.front());
    recieved_queue_.pop_front();
    return msg;
};

int Reciever::getQueueSize() {
    std::lock_guard lock(recieved_queue_mutex_);
    return recieved_queue_.size();
}

} // namespace server

