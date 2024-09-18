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
void Robot::iteration() {
    client_.iteration();
    handler_.iteration();
};

void Robot::sink(Input input) {
    /* Forward over channel. */
    std::unique_ptr<message::MessageBase> msg = std::make_unique<message::Message<message::MessageID::CMD_DRIVE>>(input);
    client_.pushSendQueue(std::move(msg));
};

Frame Robot::getFrame(double curr_time) {

};

void Robot::startStream() {

};

void Robot::stopStream() {

};

} // namespace remote