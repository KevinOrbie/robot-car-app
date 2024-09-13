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
#include "connection.h"
#include "data_type.h"
#include "logger.h"


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
// ERROR: Is not used when a baseclass is provided !!!
// template<>
// class MessageBase<Serializable> {
//    public:
//     MessageBase(Serializable &payload): payload_(payload) {};

//     void serialize(Connection& connection) {
//         payload_.serialize(connection);
//     };

//     static void deserialize(Connection& connection, Serializable& recipient) {
//         recipient.deserialize(connection);
//     };

//    private:
//     Serializable &payload_;
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
