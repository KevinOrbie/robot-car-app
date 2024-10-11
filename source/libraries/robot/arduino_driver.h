/**
 * @file arduino_driver.h
 * @author Kevin Orbie
 * 
 * @brief Declares a high level Arduino interface.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <condition_variable>
#include <mutex>

/* Custom C++ Libraries */
#include "common/timer.h"
#include "common/logger.h"
#include "common/looper.h"
#include "common/input_sink.h"

#include "arduino_socket.h"


/* ========================= Constants ========================= */
#define ARDUINO_CMD_INTERVAL_MSEC 1000 


/* ========================== Classes ========================== */
class ArduinoDriver: public InputSink, public Looper {
    enum class Throttle: uint8_t {STANDBY, FORWARD, REVERSE, BRAKE};
    enum class Direction: uint8_t {STRAIGHT, LEFT, RIGHT};

   public:
    void commandArduino();

    void iteration() override;
    void setup() override;
    
    void sink(Input input) override;

   private:
    ArduinoSocket arduino_ctrl_ = ArduinoSocket();
    std::condition_variable arduino_recently_notified_cv_;
    std::mutex arduino_recently_notified_mutex_;

    Timer stopwatch_ = Timer();

    Direction direction_ = Direction::STRAIGHT;
    Throttle throttle_ = Throttle::STANDBY;
    int speed_ = 0;
};
