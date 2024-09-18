/**
 * @file message_handler.h
 * @author Kevin Orbie
 * 
 * @brief Declares handler for messages recieved by the robot.
 */


/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "network/message_handler.h"
#include "network/server.h"


namespace robot {
using namespace message;
/* =========================== Macros ========================== */
#define PIPE_MESSAGE(msg_id) \
case msg_id: \
    Message<msg_id> *message = dynamic_cast<Message<msg_id>*>(message_base.get()); \
    on(message);    \
    break           \


/* ========================== Classes ========================== */
class MessageHandler: public server::MessageHandler {
   public:
    MessageHandler(server::Server &server): server_(server) {};

    void iteration() {
        std::unique_ptr<MessageBase> message_base = server_.popRecieveQueue();
        MessageID id = message_base->getID();

        switch (id) {
            PIPE_MESSAGE(MessageID::CMD_DRIVE);
            
            default:
                LOGW("Recieved message, with ID %d, has not handler.", static_cast<int>(id));
                break;
        }
    };

    void on(Message<MessageID::CMD_DRIVE> *msg) override;

   private:
    server::Server &server_;
};

} // namespace remote
