/**
 * @brief Message Definitions.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "message.h"


namespace message {
/* =========================== Macros ========================== */
#define CREATE_MESSAGE(msg_id, payload_t) template<> class Message<msg_id>: public MessageBase<payload_t> {}



/* ==================== Message Declarations =================== */
/* Declate Message and give it an ID. */
enum class MessageID {
    EMPTY = 0,  // Empty Message
    CMD_DRIVE   // Command the robot to update it's Drive Control State.
};


/* ==================== Message Definitions ==================== */
/* Create Specific Message Class. */
CREATE_MESSAGE(MessageID::CMD_DRIVE, bool);



} // namespace message
