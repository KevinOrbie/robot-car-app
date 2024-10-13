/**
 * @file message_handler.h
 * @author Kevin Orbie
 * 
 * @brief Declares message handler interface functions that can be defined for both 
 * the server and client handlers respectivly.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <memory>

/* Custom C++ Libraries */
#include "messages.h"


/* ========================== Classes ========================== */
namespace client {
using namespace message;
/**
 * @brief Handler for messages that could be recieved by the client.
 */
class MessageHandler {
   public:
    /* Ensure correct destruction of derived class with runtime Polymorphism. */
    virtual ~MessageHandler() = default;  

    /* Rule of Five. */
    MessageHandler()                                         = default;
    MessageHandler(MessageHandler && other)                  = default;
    MessageHandler(const MessageHandler& other)              = default;
    MessageHandler& operator=(MessageHandler && other)       = default;
    MessageHandler& operator=(const MessageHandler& other)   = default;

   public:
    // TODO: Add new messages here.
};
} // namespace client


namespace server {
using namespace message;
/**
 * @brief Handler for messages that could be recieved by the server.
 */
class MessageHandler {
   public:
    /* Ensure correct destruction of derived class with runtime Polymorphism. */
    virtual ~MessageHandler() = default;  

    /* Rule of Five. */
    MessageHandler()                                         = default;
    MessageHandler(MessageHandler && other)                  = default;
    MessageHandler(const MessageHandler& other)              = default;
    MessageHandler& operator=(MessageHandler && other)       = default;
    MessageHandler& operator=(const MessageHandler& other)   = default;
    
   public:
    virtual void on(Message<MessageID::CMD_DRIVE> *msg) {};
};
} // namespace server
