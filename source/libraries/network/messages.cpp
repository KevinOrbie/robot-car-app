/**
 * @brief Message mappings from ID to deserializer.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "messages.h"


namespace message {
/* =========================== Macros ========================== */
#define MAP_MESSAGE(msg_id) {msg_id, &Message<msg_id>::deserialize}


/* ====================== Message Mappings ===================== */
/**
 * @brief Adds this message to the deserialization mapping, to direct 
 * messages with a specifc ID to their specified deserializer.
 */
MessageBase::des_mapping_t MessageBase::deserializers_ { 

MAP_MESSAGE(MessageID::CMD_DRIVE)

};


} // namespace message
