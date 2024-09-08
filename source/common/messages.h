/**
 * @brief Header for a C++ Messaging functionality.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <memory>


/* ========================== Classes ========================== */

enum class MessageType {
    EMPTY = 0,
    NUM_MESSAGES
};

struct MessageHeader {
    MessageType type = MessageType::EMPTY;
    int num_data_bytes = 0;

    bool serialize();
    bool deserialize();
};

struct MessageData {
    MessageType type = MessageType::EMPTY;
    int num_data_bytes = 0;
    
    virtual bool serialize() = 0;
    virtual bool deserialize() = 0;
};

struct Message {
    MessageHeader header = {};
    std::unique_ptr<MessageData> data;

    bool serialize();
    bool deserialize();
};
