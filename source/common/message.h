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
#include "logger.h"
#include "connection.h"


namespace message {
/* ========================== Classes ========================== */
/* Forward Declared. */
enum class MessageID;

/**
 * @brief A template class that implements the functionality to sending a 
 * Objects of different types over a connection.
 * 
 * @tparam T: The payload-type of this message.
 * 
 * @warning Only payload-types with a corresponding template specialization 
 * defined below are supported.
 */
template<class T>
class MessageBase {
   public:
    MessageBase(T &payload): payload_(payload) {};

    void serialize(Connection& connection) {
        LOGW("Payload Type not explicitly implemented, assuming matching memory layout!");
        connection.send(reinterpret_cast<char*>(&payload_), sizeof(T));
    };

    static T deserialize(Connection& connection) {
        LOGW("Payload Type not explicitly implemented, assuming matching memory layout!");
        T payload;
        connection.recieve(reinterpret_cast<char*>(&payload), sizeof(T));
        return payload;
    };

   private:
    T &payload_;
};

/* Custom Class for Bool Message Payloads. */
// template<>
// class MessageBase<bool> {
//    public:
//     void serialize(Connection& connection, bool payload) {
//         LOGI("Bool class serialization!");
//     };

//     bool deserialize(Connection& connection) {
//         LOGI("Bool class deserialization!");
//         return true;
//     };

//     int num_data_bytes() {
//         return sizeof(bool);
//     }
// };

/* Custom Class for Serializable Message Payloads. */
// TODO: Implement


/**
 * @brief The Message template class, with wich we create a template 
 * specialization for every defined message type in messages.h. 
 */
template<MessageID ID>
class Message : public MessageBase<int> {
    static_assert(false);  // ERROR: Message corresponding with MessageID not yet created with CREATE_MESSAGE()!
};


} // namespace message
