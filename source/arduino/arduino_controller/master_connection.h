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

/* Other C++ Libraries */
#include <Arduino.h>

/* Custom C++ Libraries */
#include "arduino_types.h"


namespace arduino {
/* ========================== Classes ========================== */
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
        if (msg.id == MessageID::EMPTY) {return;};

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
        recieved_message.id = static_cast<MessageID>(cmd_buffer_[1]);
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

} // namespace arduino
