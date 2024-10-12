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
#include "common/input_sink.h"
#include "network/message_handler.h"
#include "network/server.h"


namespace robot {
using namespace message;
/* =========================== Macros ========================== */
#define PIPE_MESSAGE(msg_id) \
case msg_id: { \
    Message<msg_id> *message = dynamic_cast<Message<msg_id>*>(message_base); \
    on(message);    \
    break;          \
}


/* ========================== Classes ========================== */
class MessageHandler: public server::MessageHandler {
   public:
    MessageHandler(server::Server &server, InputSink *input_sink=nullptr): server_(server), input_sink_(input_sink) {};

    void iteration() {
        std::unique_ptr<MessageBase> message_base = server_.popRecieveQueue();
        handle(message_base.get());
    };

    /**
     * @note Can't be static, as `on()` needs to be overridden from base, which is not possible with static.
     */
    void handle(MessageBase* message_base) {
        /* No message to process? */
        if (!message_base) { return; }

        /* Get Message ID. */
        MessageID id = message_base->getID();

        /* Pipe given message to correct handler. */
        switch (id) {
            PIPE_MESSAGE(MessageID::CMD_DRIVE);
            
            default:
                LOGW("Recieved message, with ID %d, has not handler.", static_cast<int>(id));
                break;
        }
    }

    /* --------------------- Specifc Message Handlers --------------------- */
    void on(Message<MessageID::CMD_DRIVE> *msg) override;

   private:
    server::Server &server_;
    InputSink *input_sink_ = nullptr;
};

} // namespace remote
