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
#include <thread>
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

void ArduinoDriver::Reciever::flush() {
    driver_.socket().flush();
}

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

void ArduinoDriver::setupIMU() {
    LOGI("Setting up Arduino Connection...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Wait until baudrate was found for IMU
    reciever_.flush();  // Throw away old messages.
    setIMUBaudrate(arduino::BaudRate::BD_9600);
    setIMUOutputRate(arduino::OutputRate::OR_50HZ);
    setIMUContent(
        static_cast<int32_t>(arduino::Content::CNT_ACC) | 
        static_cast<int32_t>(arduino::Content::CNT_GYRO) | 
        static_cast<int32_t>(arduino::Content::CNT_ANGLE)
    );
    calibrateAccGyro();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // Give commands time to finnish before continuing.
};

/* ---------------------------- Looper Interface ---------------------------- */
void ArduinoDriver::iteration() {
    life_pulser_.iteration();
    reciever_.iteration();
};

void ArduinoDriver::thread() {
    setupIMU();
    life_pulser_.thread();
    reciever_.thread();
}

void ArduinoDriver::stop() {
    life_pulser_.stop();
    reciever_.stop();
}

/* ------------------------------- Input Sink ------------------------------- */
void ArduinoDriver::sink(Input input) {
    /* Throttle Control. */
    if (input.car_forward && input.car_backward) {
        throttle_ = Throttle::BRAKE;
    } else if (input.car_forward) {
        throttle_ = Throttle::FORWARD;
        speed_ = 255;
    } else if (input.car_backward) {
        throttle_ = Throttle::REVERSE;
        speed_ = 255;
    } else {
        throttle_ = Throttle::STANDBY;
    }

    /* Direction Control. */
    if (input.car_left && input.car_right) {
        direction_ = Direction::STRAIGHT;
    } else if (input.car_left) {
        direction_ = Direction::LEFT;
    } else if (input.car_right) {
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
        int16_t *accel = reinterpret_cast<int16_t*>(&msg.data[0]);
        setAcceleration(accel[0], accel[1], accel[2]);
        // LOGW("ACC: %f, %f, %f", 
        //     static_cast<float>(accel[0]/ 32768.0f * 16.0f), 
        //     static_cast<float>(accel[1]/ 32768.0f * 16.0f), 
        //     static_cast<float>(accel[2]/ 32768.0f * 16.0f)
        // );
        break;
    }

    case arduino::MessageID::IMU_DATA_ANGLE: {
        if (msg.data.size() != 6) {
            LOGE("Arduino Message does not contain enough data: #bytes=%d", static_cast<int>(msg.data.size()));
            return;
        }
        int16_t *angle = reinterpret_cast<int16_t*>(&msg.data[0]);
        setAngle(angle[0], angle[1], angle[2]);
        // LOGW("ANGLE: %f, %f, %f", 
        //     static_cast<float>(angle[0]/ 32768.0f * 180.0f), 
        //     static_cast<float>(angle[1]/ 32768.0f * 180.0f), 
        //     static_cast<float>(angle[2]/ 32768.0f * 180.0f)
        // );
        break;
    }

    case arduino::MessageID::IMU_DATA_GYRO: {
        if (msg.data.size() != 6) {
            LOGE("Arduino Message does not contain enough data: #bytes=%d", static_cast<int>(msg.data.size()));
            return;
        }
        int16_t *gyro = reinterpret_cast<int16_t*>(&msg.data[0]);
        setGyro(gyro[0], gyro[1], gyro[2]);
        // LOGW("GYRO: %f, %f, %f", 
        //     static_cast<float>(gyro[0]/ 32768.0f * 2000.0f), 
        //     static_cast<float>(gyro[1]/ 32768.0f * 2000.0f), 
        //     static_cast<float>(gyro[2]/ 32768.0f * 2000.0f)
        // );
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
            LOGE("(Arduino) ERROR: %c, MSG: %.*s", static_cast<char>(err), text_length, msg_text);
        } else {
            LOGE("(Arduino) ERROR: %c", static_cast<char>(err));
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

void ArduinoDriver::sendDriveCmd(uint8_t *value) {
    /* Create Command. */
    std::vector<uint8_t> command = {0};
    if (value) {
        command[0] = *value;
    } else {
        command[0] |= speed_ & 0x0F;
        command[0] |= (static_cast<uint8_t>(direction_) << 6);
        command[0] |= (static_cast<uint8_t>(throttle_) << 4);
    }
    
    /* Create Message. */
    arduino::Message msg = {};
    msg.id = arduino::MessageID::CMD_DRIVE;
    msg.data = command;

    /* Forward Message. */
    arduino_ctrl_.send(msg);

    /* Deal with timing. */
    life_pulser_.reset();
};

void ArduinoDriver::calibrateAccGyro() {
    /* Create Message. */
    arduino::Message msg = {};
    msg.id = arduino::MessageID::IMU_CALIB_ACC_GRYO;

    /* Forward Message. */
    arduino_ctrl_.send(msg);
};

void ArduinoDriver::calibrateMag() {
    /* Create Message. */
    arduino::Message msg = {};
    msg.id = arduino::MessageID::IMU_CALIB_MAG;

    /* Forward Message. */
    arduino_ctrl_.send(msg);
};

void ArduinoDriver::setIMUOutputRate(arduino::OutputRate rate) {
    /* Create Message. */
    arduino::Message msg = {};
    msg.id = arduino::MessageID::IMU_RATE;
    msg.data = {static_cast<uint8_t>(rate)};

    /* Forward Message. */
    arduino_ctrl_.send(msg);
};

void ArduinoDriver::setIMUBaudrate(arduino::BaudRate baud) {
    /* Create Message. */
    arduino::Message msg = {};
    msg.id = arduino::MessageID::IMU_BAUD;
    msg.data = {static_cast<uint8_t>(baud)};

    /* Forward Message. */
    arduino_ctrl_.send(msg);
};

void ArduinoDriver::setIMUContent(int32_t content) {
    /* Create Message. */
    arduino::Message msg = {};
    msg.id = arduino::MessageID::IMU_CONTENT;
    uint8_t *data_bytes = reinterpret_cast<uint8_t*>(&content);
    msg.data = {data_bytes[0], data_bytes[1], data_bytes[2], data_bytes[3]};

    /* Forward Message. */
    arduino_ctrl_.send(msg);
};
