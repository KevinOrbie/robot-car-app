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
#include <array>

/* Custom C++ Libraries */
#include "common/timer.h"
#include "common/logger.h"
#include "common/looper.h"
#include "common/input_sink.h"

#include "arduino_types.h"
#include "arduino_message.h"
#include "arduino_socket.h"


/* ========================= Constants ========================= */
#define ARDUINO_CMD_INTERVAL_MSEC 1000 


/* ========================== Classes ========================== */
class ArduinoDriver: public InputSink {
    enum class Throttle: uint8_t {STANDBY, FORWARD, REVERSE, BRAKE};
    enum class Direction: uint8_t {STRAIGHT, LEFT, RIGHT};

    class Reciever: public Looper {
       public:
        Reciever(ArduinoDriver &driver): driver_(driver) {};
        void iteration() override;
        void setup() override;
        void flush();

       private:
        ArduinoDriver &driver_;
    };

    class LifePulser: public Looper {
       public:
        LifePulser(ArduinoDriver &driver): driver_(driver) {};
        void iteration() override;
        void setup() override;

        void pulse();
        void reset();

       private:
        ArduinoDriver &driver_;
        Timer stopwatch_ = Timer();

        std::condition_variable arduino_recently_notified_cv_;
        std::mutex arduino_recently_notified_mutex_;
    };

   public:
    ArduinoDriver();
    
    void iteration();
    void thread();
    void stop();
    
    void sink(Input input) override;
    void handle(arduino::Message &msg);

    ArduinoSocket& socket() { return arduino_ctrl_; };

    void setAcceleration(float x, float y, float z) { accel_ = {x, y, z}; };
    void setAngle(float x, float y, float z) { angle_ = {x, y, z}; };
    void setGyro(float x, float y, float z) { gyro_ = {x, y, z}; };

    void setupIMU();
    void calibrateAccGyro();
    void calibrateMag();
    void setIMUOutputRate(arduino::OutputRate rate);
    void setIMUBaudrate(arduino::BaudRate baud);
    void setIMUContent(int32_t content);

    void sendDriveCmd(uint8_t *value = nullptr);

   private:
    ArduinoSocket arduino_ctrl_ = ArduinoSocket(ArduinoBaudRate::B9600, 10);
    LifePulser life_pulser_;
    Reciever reciever_;

    /* Drive Control. */
    Direction direction_ = Direction::STRAIGHT;
    Throttle throttle_   = Throttle::STANDBY;
    int speed_ = 0;

    /* IMU. */
    std::array<float, 3> accel_ = {0.0f,0.0f,0.0f};
    std::array<float, 3> angle_ = {0.0f,0.0f,0.0f};
    std::array<float, 3> gyro_ = {0.0f,0.0f,0.0f};
    float temperature = 0.0f;
};
