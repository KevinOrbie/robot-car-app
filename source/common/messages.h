/**
 * @brief Message Definitions.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "message.h"


namespace message {
/* =========================== Macros ========================== */
#define CREATE_MESSAGE(msg_id, payload_t) template<>                \
class Message<msg_id>: public MessageBase<payload_t> {              \
   public:                                                          \
    Message(payload_t payload): MessageBase<payload_t>(payload) {}; \
    MessageID getID() { return msg_id; };                           \
}


/* ==================== Message Declarations =================== */
/**
 * @brief Declares all message types as an enum, and as a result, 
 * assigns them an ID.
 */
enum class MessageID {
    EMPTY = 0,  // Empty Message
    CMD_DRIVE   // Command the robot to update it's Drive Control State.
};


/* ==================== Message Definitions ==================== */
/**
 * @brief Creates a template specialization of the Message<ID> class 
 * for every message type, for the associated payload type.
 */
CREATE_MESSAGE(MessageID::CMD_DRIVE, bool);



} // namespace message
