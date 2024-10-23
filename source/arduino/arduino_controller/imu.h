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

/* Other C++ Libraries */
#include <Arduino.h>
#include "wit_c_sdk_REG.h"
#include "wit_c_sdk.h"

/* Custom C++ Libraries */
#include "logger.h"


namespace arduino {
/* ========================== Defines ========================== */
/* update_flags bit positions */
#define ACC_UPDATE		0x01
#define GYRO_UPDATE		0x02
#define ANGLE_UPDATE	0x04
#define MAG_UPDATE		0x08
#define READ_UPDATE		0x80


/* ========================== Classes ========================== */

/**
 * @brief Singleton IMU class.
 * 
 * The nature of the WT901B SDK interface forces us to make this a singleton:
 *  > Global SDK register storage
 *  > Global SDK config functions
 *  > C-style SDK callbacks
 */
class IMU {
   public:
   
    /**
     * @brief Getter for singleton instance.
     */
    static IMU& instance(HardwareSerial* serial=nullptr) {
        static IMU imu(serial);  // Only constructed on first call
        return imu;
    }

    void setup() {
        WitInit(WIT_PROTOCOL_NORMAL, 0x50);
        WitSerialWriteRegister(&IMU::callbackSensorSend);
        WitRegisterCallBack(&IMU::callbackSensorData);
        WitDelayMsRegister(&IMU::callbackSensorDelay);
        autoSetupBaud();
    }

    void recieve() {
        while (serial_->available()) {
            WitSerialDataIn(serial_->read()); 
        }
    };

    void CalibrateAcceleration() {
        WitStartAccCali();
    };

    void SetBandwidth() {
        // WitSetBandwidth();
    };

    void SetOuputRate() {
        // WitSetOutputRate();
    };

    void SetBaudRate() {
        // WitSetUartBaud();
    };

    void SetContent() {
        // WitSetContent();
    };

    /**
     * @brief Get the IMU data in message format.
     * @post The returned data is set to "not updated".
     * @returns If updated data is available returns the corresponding data message, otherwise returns an empty message.
     */
    Message getDataMessage() {
        Message data_msg = {};

        if (update_flags & ACC_UPDATE) {
            data_msg.id = MessageID::IMU_DATA_ACC;
            data_msg.data = reinterpret_cast<uint8_t*>(&accel[0]);
            data_msg.num_data_bytes = sizeof(accel);
            update_flags = update_flags & ~static_cast<uint8_t>(ACC_UPDATE);
        } else if (update_flags & GYRO_UPDATE) {
            data_msg.id = MessageID::IMU_DATA_GYRO;
            data_msg.data = reinterpret_cast<uint8_t*>(&gyro[0]);
            data_msg.num_data_bytes = sizeof(gyro);
            update_flags = update_flags & ~static_cast<uint8_t>(GYRO_UPDATE);
        } else if (update_flags & ANGLE_UPDATE) {
            data_msg.id = MessageID::IMU_DATA_ANGLE;
            data_msg.data = reinterpret_cast<uint8_t*>(&angle[0]);
            data_msg.num_data_bytes = sizeof(angle);
            update_flags = update_flags & ~static_cast<uint8_t>(ANGLE_UPDATE);
        } else {
            update_flags = 0;
        }

        return data_msg;
    }

    /**
     * @brief Indicates whether updated data is available.
     */
    bool available() {
        return (update_flags != 0);
    }

   private:
    /* Private constructor for singleton. */
    IMU(HardwareSerial* serial): serial_(serial) {};
    ~IMU() = default;

    /* Rule of five. */
    IMU(IMU &other) = delete;
    IMU(IMU &&other) = delete;
    IMU& operator=(IMU &other) = delete;
    IMU& operator=(IMU &&other) = delete;

    void autoSetupBaud() {
        const uint32_t bauds[7] = {4800, 9600, 19200, 38400, 57600, 115200, 230400};
        constexpr int num_bauds = sizeof(bauds)/sizeof(bauds[0]);
	
        /* Loop over all possible baudrates. */
        for(int baud_index = 0; baud_index < num_bauds; baud_index++) {
            serial_->begin(bauds[baud_index]);
            serial_->flush();
            update_flags = 0;

            for(int attemps = 2; attemps > 0; attemps--) {
                WitReadReg(AX, 3);
                delay(200);

                recieve();

                /* Succesful Data Update ? */
                if(available()) {
                    Logger::instance().info("Baudrate Found!");
                    return ;
                }
            } 	
        }

        Logger::instance().error(ErrorID::IMU_NOT_DETECTED);
    };

    static void callbackSensorDelay(uint16_t ms) {
        delay(ms);
    };

    static void callbackSensorSend(uint8_t *p_data, uint32_t uiSize) {
        IMU::instance().serial_->write(p_data, uiSize);
        IMU::instance().serial_->flush();
    };

    static void callbackSensorData(uint32_t uiReg, uint32_t uiRegNum) {
        for(int i = 0; i < uiRegNum; i++) {
            switch(uiReg) {
                case AZ:
                    IMU::instance().update_flags |= ACC_UPDATE;
                    break;
                case GZ:
                    IMU::instance().update_flags |= GYRO_UPDATE;
                    break;
                case HZ:
                    IMU::instance().update_flags |= MAG_UPDATE;
                    break;
                case Yaw:
                    IMU::instance().update_flags |= ANGLE_UPDATE;
                    break;
                default:
                    IMU::instance().update_flags |= READ_UPDATE;
                    break;
            }
            uiReg++;
        }
    };

   private:
    HardwareSerial* serial_;

    uint8_t update_flags = 0;
    uint16_t temperature = 0;
    uint16_t angle[3] = {0, 0, 0};
    uint16_t accel[3] = {0, 0, 0};
    uint16_t gyro[3]  = {0, 0, 0};
};

} // namespace arduino
