/**
 * @file server_handler.cpp
 * @author Kevin Orbie
 * 
 * @brief Implements the message handlers for messages recieved by the server for this project.
 */

/* ========================== Include ========================== */
#include "server.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "messages.h"


namespace server {
/* ==================== Handler Multiplexer ==================== */

void Server::pipeMessage(message::MessageID id) {
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
template<> void Server::handleMessage<message::MessageID::CMD_DRIVE>() {
    using namespace message;

    bool value = Message<MessageID::CMD_DRIVE>::deserialize(connection_);
    LOGI("Recieved CMD_Drive Message: %s", (value) ? "true" : "false");
}

} // namespace server

