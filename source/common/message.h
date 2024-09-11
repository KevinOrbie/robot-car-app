/**
 * @brief Header for our C++ Messaging functionality.
 */

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
    void serialize(Connection& connection, T payload) {LOGW("Not yet implemented MessageBase type!");};
    T deserialize(Connection& connection) {LOGW("Not yet implemented MessageBase type!");};
};

/* Custom Class for Bool Message Payloads. */
template<>
class MessageBase<bool> {
   public:
    void serialize(Connection& connection, bool payload) {
        LOGI("Bool class serialization!");
    };

    bool deserialize(Connection& connection) {
        LOGI("Bool class deserialization!");
        return true;
    };
};

/* Custom Class for Float Message Payloads. */
template<>
class MessageBase<float> {
   public:
    void serialize(Connection& connection, float payload) {
        LOGI("Float class serialization!");
    };

    float deserialize(Connection& connection) {
        LOGI("Float class deserialization!");
        return 0;
    };
};

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
