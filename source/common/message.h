/**
 * @brief Header for a C++ Messaging functionality.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <cassert>
#include <memory>

/* Custom C++ Libraries */
#include "connection.h"


/* ========================== Defines ========================== */

enum class MessageType {
    EMPTY = 0,
    CMD_DRIVE = 1,
    NUM_MESSAGES
};


/* ========================== Classes ========================== */

struct MessageHeader {
    MessageType type = MessageType::EMPTY;
    int num_data_bytes = 0;

    bool serialize(const Connection &connection) const;
    bool deserialize(const Connection &connection);
};

struct MessageData {
    MessageType type = MessageType::EMPTY;
    int num_data_bytes() const;
    
    virtual bool serialize(const Connection &connection) const = 0;
    virtual bool deserialize(const Connection &connection) = 0;
};

/**
 * @note Messages are not meant to keep around, but just as a converient wrapper around the packet.
 */
struct Message {
    Message(const MessageData *data=nullptr): data{data}{
        header.type = data->type;
        header.num_data_bytes = data->num_data_bytes();
    }

    /* Prevent Moving OR Copying. */
    Message(const Message& other)            = delete;
    Message& operator=(const Message& other) = delete;
    Message(Message&& other)                 = delete;
    Message& operator=(Message&& other)      = delete;
    ~Message()                               = default;

    MessageHeader header = {};
    const MessageData *data = nullptr;  /* Doesn't own the data */

    void serialize(const Connection &connection) {
        assert(data->type == header.type && "Header MessageType is not equal to Data MessageType.");
        header.serialize(connection);
        data->serialize(connection);
    };

    /**
     * @note Needs to return a new message, as you can't pass it a MessageData object, 
     * because it is abstract.
     */
    std::unique_ptr<MessageData> deserialize(const Connection &connection) {
        std::unique_ptr<MessageData> recieved_data = nullptr;

        /* Get Message Metadata. */
        header.deserialize(connection);

        /* Assign the correct MessageData type. */
        switch (header.type) {
            case MessageType::CMD_DRIVE:
                /* code */
                break;
            
            default:
                break;
        }

        /* Deserialize the MessageData. */
        recieved_data->deserialize(connection);

        return recieved_data;
    };
};


/* ========================== Defines ========================== */
struct TextMessage: public MessageData {
    TextMessage(std::string str): text(str){};

    bool serialize(const Connection &connection) const override {
        int size = text.size();
        char *ch = (char*) text.c_str();

        connection.send(reinterpret_cast<char*>(&size), sizeof(size));
        connection.send(ch, size);
    };

    bool deserialize(const Connection &connection) override {
        char size[sizeof(int)];
        connection.recieve(size, sizeof(size));

        // char text_buffer[reinterpret_cast<int>(size)];
        // connection.recieve(text_buffer, sizeof(text_buffer));

        // text = std::string(text_buffer);
    };

   public:
    std::string text;
};

