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
#include "common/utils.h"  // gettid()
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
class MessageHandler: public server::MessageHandler, public Looper {
   public:
    MessageHandler(Reciever *recv, InputSink *input_sink=nullptr): message_reciever_(recv), input_sink_(input_sink) {};

    void iteration() {
        /* If running in seperate thread, block this thread block until message vailable. */
        if (threaded()) {
            bool message_available = message_reciever_->waitForMessage(1000);

            /* Wait again on timeout, needed to stop looper thread if requested. */
            if (!message_available) { return; }
        }

        do { /* Process all queued messages. */
            std::unique_ptr<MessageBase> message_base = message_reciever_->popRecieveQueue();
            handle(message_base.get());
        } while (message_reciever_->getQueueSize() > 0);
    };

    void setup() {
        LOGI("Running MessageHandler (TID = %d)", gettid());
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
    Reciever *message_reciever_;
    InputSink *input_sink_ = nullptr;
};

} // namespace remote
