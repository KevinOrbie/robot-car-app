/**
 * @brief C++ Socket POSIX socket interface.
 * @link https://www.linuxhowtos.org/C_C++/socket.htm
 */

/* ========================== Include ========================== */
#include "message_transciever.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None


/* ========================= Functions ========================= */
static void error(char *msg) {
  perror(msg);
  exit(1);
}


namespace message {
/* ========================== Classes ========================== */

template<MessageID ID>
void Transciever::send(Message<ID> msg) {
    if (!connection_.valid()) {
        LOGW("Invalid Connection! Could be because it is not yet initialized.");
    }

    /* Send Header. */
    sendMessageID(msg.getID());
    
    /* Send Message. */
    msg.serialize(connection_);
};

/* Explicit instantiations (because this is a library) */
template void Transciever::send<MessageID::CMD_DRIVE>(Message<MessageID::CMD_DRIVE> msg);

void Transciever::recieve() {
    if (!connection_.valid()) {
        LOGW("Invalid Connection! Could be because it is not yet initialized.");
    }

    /* Recieve Header. */
    MessageID id = recieveMessageID();
    
    /* Recieve Message & Process Further. */
    pipeMessage(id);
};


void Transciever::sendMessageID(MessageID id) {
    LOGI("Sending MessageID: %d", static_cast<int>(id));
    int message_id = static_cast<int>(id);
    char* type_buffer = reinterpret_cast<char*>(&message_id);
    int num_bytes = sizeof(int);
    connection_.send(type_buffer, num_bytes);
}

MessageID Transciever::recieveMessageID() {
    LOGI("Recieving MessageID...");
    int message_id = -1;
    connection_.recieve(reinterpret_cast<char*>(&message_id), sizeof(int));
    MessageID id = static_cast<MessageID>(message_id);
    LOGI("Recieved MessageID: %d", static_cast<int>(id));
    return id;
}

} // namespace server

