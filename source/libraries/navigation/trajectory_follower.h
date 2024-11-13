/**
 * @file trajectory_follower.h
 * @author Kevin Orbie
 * 
 * @brief Defines a trajectory follower class, generates the drive inputs required follow a specifc trajectory.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <cmath>

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "common/input_source.h"
#include "common/pose.h"
#include "node_trajectory.h"


/* =========================== Class =========================== */
class TrajectoryFollower: public InputSource {
   public:
    TrajectoryFollower(PoseProvider *pose_provider=nullptr, NodeTrajectory trajectory={}): pose_provider_(pose_provider), trajectory_(trajectory) {};

    // TODO: Add option to make trajectory circular, maybe do in Node Trajectory ?

    Input getInput() {
        if (!pose_provider_) {
            return {};
        }

        Pose pose_WOcurr = pose_provider_->getPose(common::now());
        position_t pos_WOcurr_W = pose_WOcurr.getPosition();
        position_t pos_WOnext_W = trajectory_.nextNode(pos_WOcurr_W);

        position_t pos_WFront_W = pose_WOcurr.UnitX();
        position_t pos_OcOn_W = pos_WOnext_W - pos_WOcurr_W;  // Direction (curr -> next)

        /* Offsets to reach target. */
        double rotation_offset = getYAngle(pos_WFront_W, pos_OcOn_W);
        double position_offset = pos_OcOn_W.norm();

        /* First rotate. */
        static bool rotating = false;  // Added rotation trigger hysteresis, to avoid constant corrections.
        if (abs(rotation_offset) > ROTATION_TOLERANCE_MAX || 
            (rotating && abs(rotation_offset) > ROTATION_TOLERANCE_MIN)) {
            if (rotation_offset < 0) {
                control_input_.keys[Button::RIGHT].updateState(true);
                control_input_.keys[Button::LEFT].updateState(false);
            } else {
                control_input_.keys[Button::RIGHT].updateState(false);
                control_input_.keys[Button::LEFT].updateState(true);
            }

            rotating = true;
            return control_input_;
        } else {
            control_input_.keys[Button::RIGHT].updateState(false);
            control_input_.keys[Button::LEFT].updateState(false);
            rotating = false;
        }

        /* If rotation sufficient, move. */
        if (position_offset > POSITION_TOLERANCE) {
            control_input_.keys[Button::UP].updateState(true);
        } else {
            control_input_.keys[Button::UP].updateState(false);
        }
        
        return control_input_;
    };

   private:
    /**
     * @brief Returns an angle in the [-PI, PI] range, where a positive angle is a rotation to the left.
     */
    static double getYAngle(Eigen::Vector3d vec1, Eigen::Vector3d vec2) {
        /* Project to X-Z plane. */
        vec1[1] = 0;
        vec2[1] = 0;

        /* Calculate the Angle. */
        double dot = vec1.dot(vec2);  // ~cos(angle)
        Eigen::Vector3d cross = vec1.cross(vec2); // ~sin(angle)
        double angle = asin(abs(cross.norm()) / vec1.norm() / vec2.norm());  // Range [0, PI/2] (quadrant 2)

        /* Move to quadrant 4? */
        if (dot < 0) {
            angle += M_PI_2;
        }

        /* Move to quadrant 1, 3? */
        if (cross[1] < 0) {
            angle = -angle;
        }
        
        return angle;
    }

   private:
    Input control_input_ = {};

    PoseProvider *pose_provider_ = nullptr;
    NodeTrajectory trajectory_ = {{}};

    const double ROTATION_TOLERANCE_MIN = 0.0174533 * 0.5; // 0.5 Degrees
    const double ROTATION_TOLERANCE_MAX = 0.0174533 * 5; // 5 Degrees
    const double POSITION_TOLERANCE = 0.005;          // 2 cm
};

