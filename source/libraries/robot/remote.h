/**
 * @file remote.h
 * @author Kevin Orbie
 * 
 * @brief Declares the remote gui class (simulates a direct connection to the remote).
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <memory>

/* Custom C++ Libraries */
#include "common/looper.h"
#include "common/input_sink.h"
#include "network/server.h"
#include "message_handler.h"


namespace robot {
/* ========================== Classes ========================== */
class Remote final: public server::Server {
   public:
    Remote(int port, InputSink *input_sink=nullptr): server::Server(port), input_sink_(input_sink){};
    void connect() override;
    
    /* Looper Interface. */
    void iteration() override;
    void thread() override;
    void start();
    void stop() override;

   private:
    std::unique_ptr<MessageHandler> message_handler_;
    InputSink* input_sink_;
};

// TODO: Update / Make Software Diagrams

} // namespace robot
