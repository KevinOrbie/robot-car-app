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
/* ========================== Constants ========================== */
/* update_flags bit positions */
#define FLAG_BIT_UPDATE_ACC		0x01
#define FLAG_BIT_UPDATE_GYRO	0x02
#define FLAG_BIT_UPDATE_ANGLE	0x04
#define FLAG_BIT_UPDATE_MAG		0x08
#define FLAG_BIT_UPDATE_READ	0x80


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

    /**
     * @brief The accelerometer calibration is used to remove the zero bias of the accelerometer. 
     * @note Keep the Module horizontally stationary
     */
    void calibrateAcceleration() {
        int32_t result; 
        Logger::instance().info("Calibrating Acc and Gyro!");
        result = WitStartAccCali();
        if (result != WIT_HAL_OK) { Logger::instance().error(ErrorID::OPERATION_FAILED); return; }

        delay(5000);

        result = WitStopAccCali();
        if (result != WIT_HAL_OK) { Logger::instance().error(ErrorID::OPERATION_FAILED); return; }
    };

    /**
     * @brief Magnetic field calibration is used to remove the magnetic field sensor's zero offset.
     * 
     * @note Manually rotate the robot 
     * @note Needs to be >20cm away from magnetic disturbances when called.
     * @note If it is not calibrated, it will bring about a large measurement error and affect the 
     * accuracy of the Z-axis angle measurement of the heading angle.
     */
    void calibrateMagnetometer() {
        int32_t result; 
        Logger::instance().info("Calibrating Mag!");
        result = WitStartMagCali();
        if (result != WIT_HAL_OK) { Logger::instance().error(ErrorID::OPERATION_FAILED); return; }

        delay(5000);

        result = WitStopMagCali();
        if (result != WIT_HAL_OK) { Logger::instance().error(ErrorID::OPERATION_FAILED); return; }
    };

    /**
     * @brief Sets the UART baudrate in an alternative manner?
     */
    void setBandwidth(Bandwidth bandwidth) {
        int32_t result = WitSetBandwidth(static_cast<int32_t>(bandwidth));
        if (result != WIT_HAL_OK) { Logger::instance().error(ErrorID::OPERATION_FAILED); }
    };

    /**
     * @brief Set the rate at which the IMU outputs data (all data included).
     */
    void setOutputRate(OutputRate outputrate) {
        Logger::instance().info("Setting Outputrate!");
        int32_t result = WitSetOutputRate(static_cast<int32_t>(outputrate));
        if (result != WIT_HAL_OK) { Logger::instance().error(ErrorID::OPERATION_FAILED); }
    };

    /**
     * @brief Set the symbolrate of the connection between Arduino and IMU.
     */
    void setBaudRate(BaudRate baudrate) {
        Logger::instance().info("Setting Baudrate!");
        const uint32_t bauds[8] = {0, 4800, 9600, 19200, 38400, 57600, 115200, 230400};
        int32_t result = WitSetUartBaud(static_cast<int32_t>(baudrate));
        if (result != WIT_HAL_OK) { Logger::instance().error(ErrorID::OPERATION_FAILED); }
        else { serial_->begin(bauds[static_cast<int>(baudrate)]); }
    };

    /**
     * @brief Set which content data to recieve repeatedly.
     * @note To set what content to recieve, use the Content enum values.
     */
    void setContent(int32_t content_flags) {
        Logger::instance().info("Setting Content!");
        int32_t result = WitSetContent(content_flags);
        if (result != WIT_HAL_OK) { Logger::instance().error(ErrorID::OPERATION_FAILED); }
    };

    /**
     * @brief Get the IMU data in message format.
     * @post The returned data is set to "not updated".
     * @returns If updated data is available returns the corresponding data message, otherwise returns an empty message.
     */
    Message getDataMessage() {
        Message data_msg = {};

        if (update_flags & FLAG_BIT_UPDATE_ACC) {
            data_msg.id = MessageID::IMU_DATA_ACC;
            data_msg.data = reinterpret_cast<uint8_t*>(&sReg[AX]);
            data_msg.num_data_bytes = sizeof(sReg[AX]) * 3;
            update_flags = update_flags & ~static_cast<uint8_t>(FLAG_BIT_UPDATE_ACC);
        } else if (update_flags & FLAG_BIT_UPDATE_GYRO) {
            data_msg.id = MessageID::IMU_DATA_GYRO;
            data_msg.data = reinterpret_cast<uint8_t*>(&sReg[GX]);
            data_msg.num_data_bytes = sizeof(sReg[GX]) * 3;
            update_flags = update_flags & ~static_cast<uint8_t>(FLAG_BIT_UPDATE_GYRO);
        } else if (update_flags & FLAG_BIT_UPDATE_ANGLE) {
            data_msg.id = MessageID::IMU_DATA_ANGLE;
            data_msg.data = reinterpret_cast<uint8_t*>(&sReg[Roll]);
            data_msg.num_data_bytes = sizeof(sReg[Roll]) * 3;
            update_flags = update_flags & ~static_cast<uint8_t>(FLAG_BIT_UPDATE_ANGLE);
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
                    IMU::instance().update_flags |= FLAG_BIT_UPDATE_ACC;
                    break;
                case GZ:
                    IMU::instance().update_flags |= FLAG_BIT_UPDATE_GYRO;
                    break;
                case HZ:
                    IMU::instance().update_flags |= FLAG_BIT_UPDATE_MAG;
                    break;
                case Yaw:
                    IMU::instance().update_flags |= FLAG_BIT_UPDATE_ANGLE;
                    break;
                default:
                    IMU::instance().update_flags |= FLAG_BIT_UPDATE_READ;
                    break;
            }
            uiReg++;
        }
    };

   private:
    HardwareSerial* serial_;
    bool calibrating_ = false;

    uint8_t update_flags = 0;
    // uint16_t temperature = 0;
    // uint16_t angle[3] = {0, 0, 0};
    // uint16_t accel[3] = {0, 0, 0};
    // uint16_t gyro[3]  = {0, 0, 0};
};

} // namespace arduino
