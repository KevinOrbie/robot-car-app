/**
 * @file robot.cpp
 * @author Kevin Orbie
 * 
 * @brief Defines the remote robot class (simulates a direct connection to the robot).
 */

/* ========================== Include ========================== */
#include "robot.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <memory>

/* Custom C++ Libraries */
#include "common/logger.h"


namespace remote {
/* ========================== Classes ========================== */
void Robot::connect() {
    client::Client::connect();
    message_handler_ = std::make_unique<MessageHandler>(message_reciever_.get());
};

void Robot::iteration() {
    client::Client::iteration();
    message_handler_->iteration();
};

void Robot::thread() {
    client::Client::thread();
    message_handler_->thread();
};

void Robot::stop() {
    LOGI("Stopping Robot!");
    client::Client::stop();
    LOGI("Stopping Message Handler!");
    message_handler_->stop();
};

void Robot::sink(Input input) {
    /* Forward over channel. */
    std::unique_ptr<message::MessageBase> msg = std::make_unique<message::Message<message::MessageID::CMD_DRIVE>>(input);
    message_transmitter_->pushSendQueue(std::move(msg));
};

Frame Robot::getFrame(double curr_time, PixelFormat fmt) {
    return {};
};

void Robot::startStream() {
    return;
};

void Robot::stopStream() {
    return;
};

} // namespace remote