/**
 * @file master_connection.h
 * @author Kevin Orbie
 * 
 * @brief Declares the Master Connection interface.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include <Arduino.h>


/* ========================== Classes ========================== */
struct Message {
    enum class ID: uint8_t {
        EMPTY,

        /* Master to MCU messages. */
        CMD_DRIVE = static_cast<uint8_t>('A'), 
        IMU_CALIB_ACC, 
        IMU_CALIB_MAG, 
        IMU_BAUD, 
        IMU_RATE, 
        IMU_CONTENT,

        /* MCU to Master messages. */
        IMU_DATA_ACC = static_cast<uint8_t>('a'),
        IMU_DATA_GYRO,
        IMU_DATA_ANGLE,
        ERROR
    };

    ID id = ID::EMPTY;
    uint8_t *data = nullptr;
    int num_data_bytes = 0;
};


class MasterConnection {
   public:
    MasterConnection(HardwareSerial& serial, const int baudrate=9600): serial_(serial), baudrate_(baudrate) {};

    void setup() {
        serial_.begin(baudrate_);
    }

    void recieve() {
        /* Recieve a single character. */
        if (serial_.available()) {
            cmd_buffer_[cmd_buffer_index_++] = serial_.read();

            /* Buffer overflow ? */
            if (cmd_buffer_index_ >= sizeof(cmd_buffer_) / sizeof(uint8_t)) {
                cmd_buffer_index_ = 0;
            }

            // TODO: find a way to deal with '>' in data

            /* Valid start character ? */
            if (cmd_buffer_[0] != '<') {
                cmd_buffer_index_ = 0;
            }
        }
    };

    void send(Message msg) {
        /* Ignore empty messages. */
        if (msg.id == Message::ID::EMPTY) {return;};

        /* Send message. */
        serial_.write('<');
        serial_.write(static_cast<char>(msg.id));
        serial_.write(msg.data, msg.num_data_bytes);
        serial_.write('>');
    };

    Message getMessage() {
        int last_byte_index = cmd_buffer_index_ - 1;

        /* Message requires at least 3 characters. */
        if (last_byte_index < 2) {
            return {};
        }

        /* Message not yet ended. */
        if (cmd_buffer_[last_byte_index] != static_cast<uint8_t>('>')) {
            return {};
        }

        /* Create message. */
        Message recieved_message = {};
        recieved_message.id = static_cast<Message::ID>(cmd_buffer_[1]);
        recieved_message.data = &cmd_buffer_[0] + 2;
        recieved_message.num_data_bytes = last_byte_index - 2;

        /* Reset MSG Buffer. */
        cmd_buffer_index_ = 0;

        return recieved_message;
    }

   private:
    HardwareSerial& serial_;
    int baudrate_;

    uint8_t cmd_buffer_index_ = 0;
    uint8_t cmd_buffer_[64] = {};            // Max message size of 60 databytes (15x int / 7x double)
};
