/**
 * @file drive_controller.h
 * @author Kevin Orbie
 * 
 * @brief Declares a high level Driver Controller Interface.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include <Arduino.h>


namespace arduino {
/* ========================== Classes ========================== */
enum class Throttle: uint8_t {STANDBY, FORWARD, REVERSE, BRAKE};
enum class Direction: uint8_t {STRAIGHT, LEFT, RIGHT};

class DriveController {
    struct state_ {
        Direction direction   = Direction::STRAIGHT;  // Direction state_
        Throttle  throttle    = Throttle::STANDBY;    // Throttle state_
        int pwm               = 0;                    // PWM value for speed control
    };

   public:
    DriveController(
        const int pin_speed_right, 
        const int pin_speed_left, 
        const int pin_direction_right,
        const int pin_direction_left,
        const long timeout_ms = 2000
    ) : 
        PIN_SPEED_RIGHT_(pin_speed_right), 
        PIN_SPEED_LEFT_(pin_speed_left), 
        PIN_DIRECTION_RIGHT_(pin_direction_right), 
        PIN_DIRECTION_LEFT_(pin_direction_left),
        TIMEOUT_MS_(timeout_ms)
    {};

    void setup() {
        pinMode(PIN_SPEED_RIGHT_    , OUTPUT);
        pinMode(PIN_SPEED_LEFT_     , OUTPUT);
        pinMode(PIN_DIRECTION_RIGHT_, OUTPUT);
        pinMode(PIN_DIRECTION_LEFT_ , OUTPUT);
    };

    void setState(Message msg) {
        uint8_t cmd = *msg.data;

        /* Direction Control Bytes (Bytes: 0bxx000000). */
        switch((cmd & 0xC0) >> 6) {
            case 0: state_.direction = Direction::STRAIGHT; break;
            case 1: state_.direction = Direction::LEFT;     break;
            case 2: state_.direction = Direction::RIGHT;    break;
            case 3: state_.direction = Direction::STRAIGHT; break;

            default: state_.direction = Direction::STRAIGHT; break;
        }

        /* Throttle Control Bytes (Bytes: 0b00xx0000). */
        switch((cmd & 0x30) >> 4) {
            case 0: state_.throttle = Throttle::STANDBY; break;
            case 1: state_.throttle = Throttle::FORWARD; break;
            case 2: state_.throttle = Throttle::REVERSE; break;
            case 3: state_.throttle = Throttle::BRAKE;   break;
            
            default: state_.throttle = Throttle::BRAKE; break;
        }

        /* Speed Control Bytes (Bytes: 0b0000xxxx). */
        state_.pwm = (cmd & 0x0F) << 4;

        /* Mark state as updated. */
        last_cmd_time_ = millis();
        timedout_ = false;
        updated_ = true;
    }

    void iteration() {
        /* Set motor to idle if communication broken. */
        if (!timedout_ && (millis() - last_cmd_time_ > TIMEOUT_MS_)) {
            state_.direction = Direction::STRAIGHT;
            state_.throttle = Throttle::STANDBY;
            state_.pwm = 0;
            timedout_ = true;  // Make sure we only go to idle once.
            updated_ = true;   // Update Motor Commands to idle.
        }

        if (!updated_) { return; };
        
        /* Control Direction. */
        switch(state_.direction) {

        /* NOTE: Always turn at full speed and in place for now. */
        case Direction::LEFT: {
            analogWrite (PIN_SPEED_RIGHT_, 185);
            digitalWrite(PIN_DIRECTION_RIGHT_, HIGH);
            analogWrite (PIN_SPEED_LEFT_, 185);
            digitalWrite(PIN_DIRECTION_LEFT_, HIGH);
            break;
        }
        case Direction::RIGHT: {
            analogWrite (PIN_SPEED_RIGHT_, 185);
            digitalWrite(PIN_DIRECTION_RIGHT_, LOW);
            analogWrite (PIN_SPEED_LEFT_, 185);
            digitalWrite(PIN_DIRECTION_LEFT_, LOW);
            break;
        }

        /* Control Throttle. */
        case Direction::STRAIGHT: {
            switch(state_.throttle) {
            case Throttle::FORWARD: {
                analogWrite (PIN_SPEED_RIGHT_, state_.pwm);
                digitalWrite(PIN_DIRECTION_RIGHT_, HIGH);
                analogWrite (PIN_SPEED_LEFT_, state_.pwm);
                digitalWrite(PIN_DIRECTION_LEFT_, LOW);
                break;
            }
            case Throttle::REVERSE: {
                analogWrite (PIN_SPEED_RIGHT_, state_.pwm);
                digitalWrite(PIN_DIRECTION_RIGHT_, LOW);
                analogWrite (PIN_SPEED_LEFT_, state_.pwm);
                digitalWrite(PIN_DIRECTION_LEFT_, HIGH);
                break;
            }
            case Throttle::STANDBY:
            case Throttle::BRAKE:
            default: {// Normally not reached
                analogWrite (PIN_SPEED_RIGHT_, 0);
                digitalWrite(PIN_DIRECTION_RIGHT_, HIGH);
                analogWrite (PIN_SPEED_LEFT_, 0);
                digitalWrite(PIN_DIRECTION_LEFT_, LOW);
                break;
            }
            }
            break;
        }

        default: { // Normally not reached 
            analogWrite (PIN_SPEED_RIGHT_, 0);
            digitalWrite(PIN_DIRECTION_RIGHT_, HIGH);
            analogWrite (PIN_SPEED_LEFT_, 0);
            digitalWrite(PIN_DIRECTION_LEFT_, LOW);
            break;
        }
        }

        updated_ = false;
        return;
    };

   private:
    const int PIN_SPEED_RIGHT_;
    const int PIN_SPEED_LEFT_;
    const int PIN_DIRECTION_RIGHT_;
    const int PIN_DIRECTION_LEFT_;

    state_ state_ = {};
    bool updated_ = true;

    const long TIMEOUT_MS_;
    bool timedout_ = false;
    unsigned long last_cmd_time_ = 0;       // Last time we recieved a command (msec).
};

} // namespace arduino
