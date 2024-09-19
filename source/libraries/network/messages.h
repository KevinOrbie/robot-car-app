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
#include "common/input.h"
#include "message.h"


namespace message {
/* =========================== Macros ========================== */
#define ADD_MESSAGE(name) name

#define CREATE_MESSAGE(msg_id, payload_t) \
template<> class Message<msg_id>: public MessageBase {       \
   public:                                                                              \
    Message(payload_t payload): payload_(payload) {};                                   \
    MessageID getID() override { return msg_id; };                                      \
    void serialize(Connection &connection) override {payload_.serialize(connection);};  \
    static std::unique_ptr<MessageBase> deserialize(Connection& connection) {           \
        payload_t payload = Payload<payload_t>::deserialize(connection);                \
        std::unique_ptr<MessageBase> msg = std::make_unique<Message<msg_id>>(payload);  \
        return msg;                                                                     \
    }                                                                                   \
    payload_t value() {return payload_.value();}                                        \
   private:                                                                             \
    Payload<payload_t> payload_;                                                        \
};


/* ==================== Message Declarations =================== */
/**
 * @brief Declares all message types as an enum, and as a result, 
 * assigns them an ID.
 */
enum class MessageID {
    EMPTY = 0,               // Empty Message
    
ADD_MESSAGE(CMD_DRIVE)   // Command the robot to update it's Drive Control State.

};


/* ==================== Message Definitions ==================== */
/**
 * @brief Creates a template specialization of the Message<ID> class 
 * for every message type, for the associated payload type.
 */
CREATE_MESSAGE(MessageID::CMD_DRIVE, Input);


/**
 * @note New messages should also add code in messages.cpp & the 
 * corresponding message handlers.
 */


} // namespace message
