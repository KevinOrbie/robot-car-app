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
    IMU_CALIB_ACC_GRYO, 
    IMU_CALIB_MAG, 
    IMU_BW, 
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
    INVALID_MSG_DATA,
    IMU_NOT_DETECTED,
    OPERATION_FAILED
};


/* ========================== Others =========================== */

/**
 * @note Don't change, the bandwidth int values match the WIT IMU code.
 */
enum class Bandwidth: uint8_t {
    BW_256HZ,
    BW_184HZ,
    BW_94HZ,
    BW_44HZ,
    BW_21HZ,
    BW_10HZ,
    BW_5HZ
};

/**
 * @note Don't change, the baudrate int values match the WIT IMU code.
 */
enum class BaudRate: uint8_t {
    BD_4800	 = 0x01,
    BD_9600,
    BD_19200,
    BD_38400,
    BD_57600,
    BD_115200,
    BD_230400
};

/**
 * @note Don't change, the outputrate int values match the WIT IMU code.
 */
enum class OutputRate: uint8_t {
    OR_NONE	= 0x0d,
    OR_02HZ	= 0x01,
    OR_05HZ,
    OR_1HZ,
    OR_2HZ,
    OR_5HZ,
    OR_10HZ,
    OR_20HZ,
    OR_50HZ,
    OR_100HZ,
    OR_200HZ = 0x0b,
    OR_ONCE,
};

/**
 * @note Don't change, the content (RSW) int values match the WIT IMU code.
 */
enum class Content: uint16_t {
    CNT_TIME 	= 0x01,
    CNT_ACC		= 0x02,
    CNT_GYRO	= 0x04,
    CNT_ANGLE	= 0x08,
    CNT_MAG		= 0x10,
    CNT_PORT	= 0x20,  // Data Output Port Status
    CNT_PRESS	= 0x40,  // Atmospheric Pressure
    CNT_GPS		= 0x80,
    CNT_V       = 0x100, // Ground Speed
    CNT_Q       = 0x200, // Quaternion
    CNT_GSA		= 0x400, // Satellite Positioning Accuracy Output
    CNT_MASK	= 0xfff
};

} // namespace arduino
