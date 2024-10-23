/**
 * @file arduino_messages.h
 * @author Kevin Orbie
 * 
 * @brief Declares a list of arduino communication message and error types.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
// None


namespace arduino {
/* ========================= Messaging ========================= */
enum class MessageID: uint8_t {
    EMPTY,

    /* Master --> MCU messages. */
    CMD_DRIVE = static_cast<uint8_t>('A'), 
    IMU_CALIB_ACC, 
    IMU_CALIB_MAG, 
    IMU_BAUD, 
    IMU_RATE, 
    IMU_CONTENT,

    /* MCU --> Master messages. */
    IMU_DATA_ACC = static_cast<uint8_t>('a'),
    IMU_DATA_GYRO,
    IMU_DATA_ANGLE,

    INFO,
    ERROR
};

struct Message {
    MessageID id = MessageID::EMPTY;
    uint8_t *data = nullptr;
    int num_data_bytes = 0;
};


/* ========================== Errors =========================== */
enum class ErrorID: uint8_t {
    UNKOWN_MSG_ID = static_cast<uint8_t>('a'),
    IMU_NOT_DETECTED,
};

} // namespace arduino
