/**
 * @file imu.h
 * @author Kevin Orbie
 * 
 * @brief C++ wrapper for Arduino using the WT901B IMU SDK.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>
#include <string.h>

/* Standard C++ Libraries */
// None

/* Other C++ Libraries */
#include <Arduino.h>
#include "wit_c_sdk_REG.h"
#include "wit_c_sdk.h"

/* Custom C++ Libraries */
#include "master_connection.h"


namespace arduino {
/* ========================== Classes ========================== */

/**
 * @brief Singleton Logger class.
 * @note Making this a singleton allows us to give it global scope.
 */
class Logger {
   public:
    /**
     * @brief Getter for singleton instance.
     */
    static Logger& instance(MasterConnection* master = nullptr) {
        static Logger logger = Logger(master);  // Only constructed on first call
        return logger;
    }

    void error(ErrorID error, char* msg=nullptr) {
        /* Setup Message Data. */
        size_t size = (msg) ? strlen(msg) : 0;
        uint8_t data[size + 1];  // tmp stack allocated data array
        data[0] = static_cast<uint8_t>(error);
        memcpy(&data + 1, msg, size); // Copy string without terminator

        /* Setup Message. */
        Message err_msg = {};
        err_msg.id = MessageID::ERROR;
        err_msg.data = &data[0];
        err_msg.num_data_bytes = sizeof(data)/sizeof(uint8_t);
        master_->send(err_msg);
    }

    void info(char* msg=nullptr) {
        /* Setup Message Data. */
        int msg_length = strlen(msg);
        uint8_t *data_ptr = reinterpret_cast<uint8_t*>(msg);

        /* Setup Message. */
        Message info_msg = {};
        info_msg.id = MessageID::INFO;
        info_msg.data = data_ptr;
        info_msg.num_data_bytes = msg_length;
        master_->send(info_msg);
    }

   private:
    Logger(MasterConnection* master) : master_(master) {}; 

   private:
    MasterConnection *master_ = nullptr;
};

} // namespace arduino
