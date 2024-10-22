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

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include <Arduino.h>


/* ========================== Classes ========================== */
class IMU {
   public:
    IMU(HardwareSerial& serial, const int baudrate=9600): serial_(serial), baudrate_(baudrate) {};

    void setup() {
        serial_.begin(baudrate_);
    }

    void recieve() {
        // Recieve all data with Serial.read();
        if (serial_.available()) {
            uint8_t value = serial_.read(); 
        }
    };

   private:
    HardwareSerial& serial_;
    int baudrate_;
};