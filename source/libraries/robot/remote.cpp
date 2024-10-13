/**
 * @file remote.cpp
 * @author Kevin Orbie
 * 
 * @brief Defines the remote gui class (simulates a direct connection to the remote).
 */

/* ========================== Include ========================== */
#include "remote.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "common/logger.h"
#include "common/utils.h"  // gettid()


namespace robot {
/* ========================== Classes ========================== */
void Remote::connect() {
    server::Server::connect();
    message_handler_ = std::make_unique<MessageHandler>(message_reciever_.get(), input_sink_);
};

void Remote::iteration() {
    server::Server::iteration();
    message_handler_->iteration();
};

void Remote::thread() {
    server::Server::thread();
    message_handler_->thread();
};

void Remote::stop() {
    LOGI("Stopping Remote!");
    server::Server::stop();
    LOGI("Stopping Message Handler!");
    message_handler_->stop();
};

} // namespace robot
