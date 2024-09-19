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
class Remote: public Looper {
   public:
    Remote(int port, InputSink *input_sink=nullptr): server_(port, false), handler_(server_) {};

    void connect();
    void iteration() override;

   private:
    server::Server server_;
    MessageHandler handler_;
};


} // namespace robot