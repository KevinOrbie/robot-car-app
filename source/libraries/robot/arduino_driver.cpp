/**
 * @file arduino_driver.cpp
 * @author Kevin Orbie
 * 
 * @brief Defines a high level Arduino interface.
 */

/* ========================== Include ========================== */
#include "arduino_driver.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <condition_variable>
#include <chrono>

/* Custom C++ Libraries */
#include "common/logger.h"
#include "common/utils.h"  // gettid()


/* ========================== Classes ========================== */
void ArduinoDriver::commandArduino() {
    /* Create Command. */
    std::vector<uint8_t> command = {0};
    command[0] |= speed_ & 0x0F;
    command[0] |= (static_cast<uint8_t>(direction_) << 6);
    command[0] |= (static_cast<uint8_t>(throttle_) << 4);

    /* Forward Command. */
    arduino_ctrl_.send(command);

    /* Deal with timing. */
    stopwatch_.start();
    arduino_recently_notified_cv_.notify_all();
};

void ArduinoDriver::iteration() {
    /* Resend command if enough time has passed. */
    if (thread_.get_id() == std::thread::id()) { /* Running in main thread. */
        if (stopwatch_.stop() >= static_cast<double>(ARDUINO_CMD_INTERVAL_MSEC) / 1000.0) { commandArduino(); }

    } else {/* Running in seperate thread. */
        /* Make blocking to avoid busy waiting. */
        std::unique_lock<std::mutex> lock(arduino_recently_notified_mutex_);
        auto res = arduino_recently_notified_cv_.wait_for(lock, std::chrono::milliseconds(ARDUINO_CMD_INTERVAL_MSEC));
        /* If notified before timout reached, again wait for command interval. */
        if (res == std::cv_status::timeout) { commandArduino(); }
    }
};

void ArduinoDriver::setup() {
    LOGI("Running ArduinoDriver (TID = %d)", gettid());
};

void ArduinoDriver::sink(Input input) {
    // TODO: Make thread-safe

    /* Throttle Control. */
    if (input.keys[Button::W].held && input.keys[Button::S].held) {
        throttle_ = Throttle::BRAKE;
    } else if (input.keys[Button::W].held) {
        throttle_ = Throttle::FORWARD;
        speed_ = 255;
    } else if (input.keys[Button::S].held) {
        throttle_ = Throttle::REVERSE;
        speed_ = 255;
    } else {
        throttle_ = Throttle::STANDBY;
    }

    /* Direction Control. */
    if (input.keys[Button::A].held && input.keys[Button::D].held) {
        direction_ = Direction::STRAIGHT;
    } else if (input.keys[Button::A].held) {
        direction_ = Direction::LEFT;
    } else if (input.keys[Button::D].held) {
        direction_ = Direction::RIGHT;
    } else {
        direction_ = Direction::STRAIGHT;
    }

    /* Notify Arduino. */
    commandArduino();
};
