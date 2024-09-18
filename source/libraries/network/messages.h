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
#define ADD_MESSAGE(name) name
#define MAP_MESSAGE(msg_id) {msg_id, &Message<msg_id>::deserialize}

#define CREATE_MESSAGE(msg_id, payload_t) \
template<> class Message<msg_id>: public MessageBase, public Payload<payload_t> {       \
   public:                                                                              \
    Message(payload_t payload): Payload<payload_t>(payload) {};                         \
    MessageID getID() { return msg_id; };                                               \
    static std::unique_ptr<MessageBase> deserialize(Connection& connection) {           \
        payload_t payload = Payload<payload_t>::deserialize(connection);                \
        std::unique_ptr<MessageBase> msg = std::make_unique<Message<msg_id>>(payload);  \
        return msg;                                                                     \
    }                                                                                   \
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
CREATE_MESSAGE(MessageID::CMD_DRIVE, bool);


/* ====================== Message Mappings ===================== */
/**
 * @brief Adds this message to the deserialization mapping, to direct 
 * messages with a specifc ID to their specified deserializer.
 */
MessageBase::des_mapping_t MessageBase::deserializers_ { 

MAP_MESSAGE(MessageID::CMD_DRIVE)

};


} // namespace message
