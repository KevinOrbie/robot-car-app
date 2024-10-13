/**
 * @brief Header for our C++ Messaging functionality.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <functional>
#include <stdexcept>
#include <typeinfo>
#include <memory>
#include <map>

/* Custom C++ Libraries */
#include "common/logger.h"
#include "connection.h"


namespace message {
/* ========================== Classes ========================== */
/* Forward Declared. */
enum class MessageID;

/**
 * @brief Base class for all Messages, which defines a common interface.
 */
class MessageBase {
    typedef std::map<MessageID, std::unique_ptr<MessageBase>(*)(Connection&)> des_mapping_t;
    static des_mapping_t deserializers_;

   public:
    virtual ~MessageBase() = default;  /* Runtime Polymorphism. */

    /* Rule of Five. */
    MessageBase()                                      = default;
    MessageBase(MessageBase && other)                  = default;
    MessageBase(const MessageBase& other)              = default;
    MessageBase& operator=(MessageBase && other)       = default;
    MessageBase& operator=(const MessageBase& other)   = default;

    virtual void serialize(Connection &connection) = 0;
    virtual MessageID getID() = 0;

    static std::unique_ptr<MessageBase> deserialize(MessageID id, Connection &connection) {
        try {
            return deserializers_.at(id)(connection);
        } catch(const std::out_of_range& oor) {
            LOGW("Recieved message with ID %d has no derserializer: %s", static_cast<int>(id), oor.what());
        }

        return nullptr;
    };
};

/**
 * @brief A template class that implements the functionality to sending a 
 * Objects of different types over a connection.
 * 
 * @tparam T: The payload-type of this message.
 * 
 * @note We can make use of template specialization if needed.
 */
template<class T>
class Payload {
   public:
    Payload(T &payload): payload_(payload) {};

    void serialize(Connection& connection) {
        static bool printed = false;
        if (!printed) {
            LOGW("Payload Type not explicitly implemented, assuming matching memory layout!");
            printed = true;
        }

        connection.send(reinterpret_cast<char*>(&payload_), sizeof(T));
    };

    static T deserialize(Connection& connection) {
        static bool printed = false;
        if (!printed) {
            LOGW("Payload Type not explicitly implemented, assuming matching memory layout!");
            printed = true;
        }

        T payload;
        /* TODO: possbily make not busy waiting*/
        while(!connection.recieve(reinterpret_cast<char*>(&payload), sizeof(T))) {};
        return payload;
    };

    T value() {return payload_;};

   private:
    T payload_;
};

/**
 * @brief The Message template class, with wich we create a template 
 * specialization for every defined message type in messages.h. 
 */
template<MessageID ID>
class Message : public MessageBase {
    // static_assert(false);  // ERROR: Message corresponding with MessageID not yet created with CREATE_MESSAGE()!
};


} // namespace message
