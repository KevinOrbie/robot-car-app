/**
 * @file message_handler.cpp
 * @author Kevin Orbie
 * 
 * @brief Defines handler functions for messages recieved by the server.
 */


/* ========================== Include ========================== */
#include "message_handler.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "common/logger.h"


namespace robot {
/* ========================== Classes ========================== */
void MessageHandler::on(Message<MessageID::CMD_DRIVE> *msg) {
    // LOGI("Command Drive message recieved: W held = '%s', W pressed = '%s', W released = '%s'", 
    //       msg->value().keys[Button::W].held ? "true ": "false", 
    //       msg->value().keys[Button::W].pressed ? "true ": "false", 
    //       msg->value().keys[Button::W].released ? "true ": "false"
    // );
    if (input_sink_) {
        input_sink_->sink(msg->value());
    }
};

} // namespace remote