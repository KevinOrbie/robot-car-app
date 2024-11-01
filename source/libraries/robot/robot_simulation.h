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
#include "common/pose.h"


namespace robot {
/* ========================== Classes ========================== */
class RobotInputSimulation: public InputSink, public PoseProvider {
    enum class Throttle: uint8_t {STANDBY, FORWARD, REVERSE, BRAKE};
    enum class Direction: uint8_t {STRAIGHT, LEFT, RIGHT};

    typedef std::chrono::time_point<std::chrono::high_resolution_clock> timestamp_t;
    typedef std::array<float,3> acc_t;
    typedef std::array<float,3> vel_t;

   public:
    RobotInputSimulation() {
        last_update_timestamp_ = std::chrono::high_resolution_clock::now();
    }

    void advance(double timesdelta_seconds) {
        pose_WR_W_.translate(timesdelta_seconds * vel_W_[0], timesdelta_seconds * vel_W_[1], timesdelta_seconds * vel_W_[2]);
    }

    Pose getPose(std::chrono::time_point<std::chrono::high_resolution_clock> request_timestamp) {
        const std::lock_guard<std::mutex> lock(pose_mutex_);
        double timestep_seconds = std::chrono::duration_cast<std::chrono::microseconds>(request_timestamp - last_update_timestamp_).count() * 1e-6;
        advance(timestep_seconds);
        last_update_timestamp_ = request_timestamp;
        return pose_WR_W_;
    };

    void sink(Input input) {
        timestamp_t input_timestamp = std::chrono::high_resolution_clock::now();

        const std::lock_guard<std::mutex> lock(pose_mutex_);
        double timestep_seconds = std::chrono::duration_cast<std::chrono::microseconds>(input_timestamp - last_update_timestamp_).count() * 1e-6;

        /* Detect Control. */
        Direction direction = Direction::STRAIGHT;
        Throttle throttle = Throttle::STANDBY;

        if (input.keys[Button::LEFT].held == input.keys[Button::RIGHT].held) {
            // Both left and right on/off.
            direction = Direction::STRAIGHT;

            if (input.keys[Button::UP].held == input.keys[Button::DOWN].held) {
                // Both forward and backward on/off.
                throttle = Throttle::STANDBY;
            } else if (input.keys[Button::UP].held) {
                throttle = Throttle::FORWARD;
            } else if (input.keys[Button::DOWN].held) {
                throttle = Throttle::REVERSE;
            }

        } else if (input.keys[Button::LEFT].held) {
            direction = Direction::LEFT;
        } else if (input.keys[Button::RIGHT].held) {
            direction = Direction::RIGHT;
        }

        /* Update Kinematics. */
        if (throttle == Throttle::FORWARD) {
            vel_W_[0] = 1.0f;
        } else if (throttle == Throttle::REVERSE) {
            vel_W_[0] = -1.0f;
        } else {
            vel_W_[0] = 0.0f;
        }

        // TODO: 1) Simple cnst velocity on W system
        // TODO: 2) move to eigen for all vectors / rotations
        // TODO: 3) Use Acceleration?
        // TODO: 4) Add rotations
        // TODO: 5) Refine using actual robot measurments (via camera).

        advance(timestep_seconds);
        last_update_timestamp_ = input_timestamp;
    };

   private:
    timestamp_t last_update_timestamp_;
    std::mutex pose_mutex_;
    Pose pose_WR_W_ = {};
    acc_t acc_W_ = {}; // NOTE: does not remain constant, but depends on speed (maybe model some force m)
    vel_t vel_W_ = {}; // NOTE: 
};

// TODO: add all new files to CMakeLists.txt file

} // namespace robot
