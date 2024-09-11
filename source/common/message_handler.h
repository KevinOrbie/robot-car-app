/**
 * @brief Header for Message handling functionality.
 */

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

class RobotMessageHandler {
    void on(Message<MessageID::CMD_DRIVE>); // TODO: Implement (on the robot side);
};

class ClientMessageHandler {
    // No client messages yet.
};


} // namespace message
