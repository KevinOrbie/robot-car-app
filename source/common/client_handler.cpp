/**
 * @file client_handler.cpp
 * @author Kevin Orbie
 * 
 * @brief Implements the message handlers for messages recieved by the client for this project.
 */

/* ========================== Include ========================== */
#include "client.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "messages.h"


namespace client {
/* ==================== Handler Multiplexer ==================== */

void Client::pipeMessage(message::MessageID id) {
    using namespace message;

    switch (id) {
        /* ################ MESSAGE PIPES ################ */
        case MessageID::CMD_DRIVE:  handleMessage<MessageID::CMD_DRIVE>(); break;
        
        /* ############################################### */
        default:
            LOGW("Recieved unsupported MessageID: %d", static_cast<int>(id));
            break;
    }
}


/* ====================== Message Handlers ===================== */
template<> void Client::handleMessage<message::MessageID::CMD_DRIVE>() {
    using namespace message;

    bool value = Message<MessageID::CMD_DRIVE>::deserialize(connection_);
    LOGI("Recieved CMD_Drive Message: %d", static_cast<int>(value));
}

} // namespace client

