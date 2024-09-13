/**
 * @brief Header for our C++ Messaging functionality.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "data_type.h"
#include "messages.h"
#include "logger.h"
#include "server.h"


namespace robot {
/* ========================== Classes ========================== */

namespace local {
class DriveController {
   public:
    void process(DriveControl &state) { state_ = state; };
    void commandArduino() {
        LOGI("Sending command to Arduino!");
    };

   private:
    DriveControl state_ = {};
    // ArduinoSocket
};
} // namespace local


namespace remote {
class DriveController {
   public:
    void process(float speed) { 
        state_.speed = speed;
    };

    void send(server::Server &server) {
        server.send(message::Message<message::MessageID::CMD_DRIVE>(state_));
    };

   private:
    DriveControl state_ = {};
};
} // namespace local


} // namespace message
