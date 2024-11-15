/**
 * @file robot_simulation.h
 * @author Kevin Orbie
 * 
 * @brief Declares a bare bones robot simulation class.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <memory>
#include <mutex>

/* Custom C++ Libraries */
#include "common/input_sink.h"
#include "common/clock.h"
#include "common/pose.h"

/* ========================= Constants ========================= */
static const double LINEAR_VELOCITY = 0.39;   // m / s
static const double ANGULAR_VELOCITY = 0.79;  // rad / s


namespace robot {
/* ========================== Classes ========================== */
class RobotInputSimulation: public InputSink, public PoseProvider {
    enum class Throttle: uint8_t {STANDBY, FORWARD, REVERSE, BRAKE};
    enum class Direction: uint8_t {STRAIGHT, LEFT, RIGHT};

   public:
    RobotInputSimulation() {
        last_update_timestamp_ = common::now();
    }

    void advance(double timesdelta_seconds) {
        pose_WR_.translate(vel_W_ * timesdelta_seconds);
        pose_WR_.rotate(ang_vel_W_ * timesdelta_seconds);
    }

    Pose getPose(timestamp_t timestamp) {
        const std::lock_guard<std::mutex> lock(pose_mutex_);
        advance(common::seconds(last_update_timestamp_, timestamp));
        last_update_timestamp_ = timestamp;
        return pose_WR_;
    };

    void sink(Input input) {
        timestamp_t input_timestamp = common::now();

        const std::lock_guard<std::mutex> lock(pose_mutex_);

        /* Detect Control. */
        Direction direction = Direction::STRAIGHT;
        Throttle throttle = Throttle::STANDBY;

        if (input.car_left == input.car_right) {
            // Both left and right on/off.
            direction = Direction::STRAIGHT;

            if (input.car_forward == input.car_backward) {
                // Both forward and backward on/off.
                throttle = Throttle::STANDBY;
            } else if (input.car_forward) {
                throttle = Throttle::FORWARD;
            } else if (input.car_backward) {
                throttle = Throttle::REVERSE;
            }

        } else if (input.car_left) {
            direction = Direction::LEFT;
        } else if (input.car_right) {
            direction = Direction::RIGHT;
        }

        /* Update Kinematics. */
        if (throttle == Throttle::FORWARD) {
            vel_W_ = pose_WR_.UnitX() * LINEAR_VELOCITY;
        } else if (throttle == Throttle::REVERSE) {
            vel_W_ = pose_WR_.UnitX() * -LINEAR_VELOCITY;
        } else {
            vel_W_ = {0.0, 0.0, 0.0};
        }

        if (direction == Direction::LEFT) {
            ang_vel_W_[1] = ANGULAR_VELOCITY;
        } else if (direction == Direction::RIGHT) {
            ang_vel_W_[1] = -ANGULAR_VELOCITY;
        } else {
            ang_vel_W_ = {0.0, 0.0, 0.0};
        }

        advance(common::seconds(last_update_timestamp_, input_timestamp));
        last_update_timestamp_ = input_timestamp;
    };

   private:
    timestamp_t last_update_timestamp_;
    std::mutex pose_mutex_;

    Pose pose_WR_ = {};
    velocity_t vel_W_ = {};
    angular_velocity_t ang_vel_W_ = {};
};

// TODO: add all new files to CMakeLists.txt file

} // namespace robot
