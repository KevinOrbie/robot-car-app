/**
 * @file message_handler.h
 * @author Kevin Orbie
 * 
 * @brief Declares handler for messages recieved by a client.
 */


/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "network/message_handler.h"
#include "network/client.h"


namespace remote {
using namespace message;
/* =========================== Macros ========================== */
#define PIPE_MESSAGE(msg_id) \
case msg_id: \
    Message<msg_id> *message = dynamic_cast<Message<msg_id>*>(message_base.get()); \
    on(message);    \
    break           


/* ========================== Classes ========================== */
class MessageHandler: public client::MessageHandler {
   public:
    MessageHandler(client::Client &client): client_(client) {};

    void iteration() {
        std::unique_ptr<MessageBase> message_base = client_.popRecieveQueue();
        if (!message_base) {
            /* No message to process. */
            return;
        }
        MessageID id = message_base->getID();

        switch (id) {
            // Not yet any messages

            default:
                LOGW("Recieved message, with ID %d, has not handler.", static_cast<int>(id));
                break;
        }
    };

   private:
    client::Client &client_;
};

} // namespace remote