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


/* ====================== Internal Classes ===================== */

void ArduinoDriver::Reciever::iteration() {
    /* Recieve all Socket Bytes (blocking in case of threaded). */
    if (threaded()) { driver_.socket().wait(); };
    driver_.socket().recieve();

    /* Process all recieved messages. */
    arduino::Message msg;
    do {
        msg = driver_.socket().getMessage();
        driver_.handle(msg);
    } while (msg.id != arduino::MessageID::EMPTY);
};

void ArduinoDriver::Reciever::setup() {
    LOGI("Running ArduinoDriver Reciever (TID = %d)", gettid());
};

void ArduinoDriver::LifePulser::iteration() {
    /* Pulse if enough time has passed without life update. */
    if (!threaded()) { 
        /* Pulse when the time interval limit reached. */
        if (stopwatch_.stop() >= static_cast<double>(ARDUINO_CMD_INTERVAL_MSEC) / 1000.0) { 
            pulse(); 
        }

    } else { 
        /* Block (avoid busy waiting) for ARDUINO_CMD_INTERVAL_MSEC. */
        std::unique_lock<std::mutex> lock(arduino_recently_notified_mutex_);
        auto res = arduino_recently_notified_cv_.wait_for(lock, std::chrono::milliseconds(ARDUINO_CMD_INTERVAL_MSEC));

        /* Pulse when the waiter timed out. */
        if (res == std::cv_status::timeout) { pulse(); }
    }
};

void ArduinoDriver::LifePulser::setup() {
    LOGI("Running ArduinoDriver LifePulser (TID = %d)", gettid());
};

void ArduinoDriver::LifePulser::pulse() {
    driver_.sendDriveCmd();
};

void ArduinoDriver::LifePulser::reset() {
    if (threaded()) {
        /* Resets timeout in iteration() without pulsing. */
        arduino_recently_notified_cv_.notify_all();
    } else {
        /* Resets timeout in iteration(). */
        stopwatch_.start();
    }
};


/* ======================== Driver Class ======================= */
ArduinoDriver::ArduinoDriver(): life_pulser_(*this), reciever_(*this) {};

/* ---------------------------- Looper Interface ---------------------------- */
void ArduinoDriver::iteration() {
    life_pulser_.iteration();
    reciever_.iteration();
};

void ArduinoDriver::thread() {
    life_pulser_.thread();
    reciever_.thread();
}

void ArduinoDriver::stop() {
    life_pulser_.stop();
    reciever_.stop();
}

/* ------------------------------- Input Sink ------------------------------- */
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
    sendDriveCmd();
};


/* ---------------------------- Arduino Messaging --------------------------- */
void ArduinoDriver::handle(arduino::Message &msg) {
    switch (msg.id) {
    /* No Message Recieved */
    case arduino::MessageID::EMPTY:  
        break;

    case arduino::MessageID::IMU_DATA_ACC: {
        if (msg.data.size() != 6) {
            LOGE("Arduino Message does not contain enough data: #bytes=%d", static_cast<int>(msg.data.size()));
            return;
        }
        uint16_t *accel = reinterpret_cast<uint16_t*>(&msg.data[0]);
        setAcceleration(accel[0], accel[1], accel[2]);
        LOGW("ACC: %d, %d, %d", static_cast<int>(accel[0]), static_cast<int>(accel[1]), static_cast<int>(accel[2]));

        // TODO: update range and print floats, like:
        // fAcc[i] = sReg[AX+i] / 32768.0f * 16.0f;
		// fGyro[i] = sReg[GX+i] / 32768.0f * 2000.0f;
		// fAngle[i] = sReg[Roll+i] / 32768.0f * 180.0f;

        break;
    }

    case arduino::MessageID::IMU_DATA_ANGLE: {
        if (msg.data.size() != 6) {
            LOGE("Arduino Message does not contain enough data: #bytes=%d", static_cast<int>(msg.data.size()));
            return;
        }
        uint16_t *angle = reinterpret_cast<uint16_t*>(&msg.data[0]);
        setAngle(angle[0], angle[1], angle[2]);
        LOGW("ANGLE: %d, %d, %d", static_cast<int>(angle[0]), static_cast<int>(angle[1]), static_cast<int>(angle[2]));
        break;
    }

    case arduino::MessageID::IMU_DATA_GYRO: {
        if (msg.data.size() != 6) {
            LOGE("Arduino Message does not contain enough data: #bytes=%d", static_cast<int>(msg.data.size()));
            return;
        }
        uint16_t *gyro = reinterpret_cast<uint16_t*>(&msg.data[0]);
        setGyro(gyro[0], gyro[1], gyro[2]);
        LOGW("GYRO: %d, %d, %d", static_cast<int>(gyro[0]), static_cast<int>(gyro[1]), static_cast<int>(gyro[2]));
        break;
    }

    case arduino::MessageID::ERROR: {
        if (msg.data.size() < 1) {
            LOGE("Arduino Message does not contain enough data: #bytes=%d", static_cast<int>(msg.data.size()));
            return;
        }
        arduino::ErrorID err = static_cast<arduino::ErrorID>(msg.data[0]);

        /* Check for extra text data. */
        if (msg.data.size() >= 2) {
            char *msg_text = reinterpret_cast<char*>(&msg.data[1]);
            int text_length = msg.data.size() - 1;
            LOGE("(Arduino) ERROR:%c, MSG:%.*s", static_cast<char>(err), text_length, msg_text);
        } else {
            LOGE("(Arduino) ERROR:%c", static_cast<char>(err));
        }
        break;
    }

    case arduino::MessageID::INFO: {
        /* Check for text data. */
        if (msg.data.size() > 0) {
            char *msg_text = reinterpret_cast<char*>(&msg.data[0]);
            int text_length = msg.data.size();
            LOGI("(Arduino) INFO: %.*s", text_length, msg_text);
        } else {
            LOGI("(Arduino) INFO: null");
        }
        break;
    }
    
    default: 
      LOGE("Unrecognized MessageID: %d", static_cast<int>(msg.id));
      break;
  }
}

void ArduinoDriver::sendDriveCmd() {
    /* Create Command. */
    std::vector<uint8_t> command = {0};
    command[0] |= speed_ & 0x0F;
    command[0] |= (static_cast<uint8_t>(direction_) << 6);
    command[0] |= (static_cast<uint8_t>(throttle_) << 4);

    /* Forward Command. */
    // arduino_ctrl_.send(command);

    /* Deal with timing. */
    life_pulser_.reset();
};
