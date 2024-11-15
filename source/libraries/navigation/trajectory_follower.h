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

        /* Declare Next State's Control Variables. */
        bool set_car_backward   = false;
        bool set_car_forward    = false;
        bool set_car_right      = false;
        bool set_car_left       = false;

        /* First rotate. */
        static bool rotating = false;  // Added rotation trigger hysteresis, to avoid constant corrections.
        if (abs(rotation_offset) > ROTATION_TOLERANCE_MAX || 
            (rotating && abs(rotation_offset) > ROTATION_TOLERANCE_MIN)) {
            if (rotation_offset < 0) {
                set_car_right = true;
                set_car_left = false;
            } else {
                set_car_right = false;
                set_car_left = true;
            }

            rotating = true;
        } else {
            set_car_right = false;
            set_car_left = false;
            rotating = false;
        }

        /* If rotation sufficient, move. */
        if (!rotating && position_offset > POSITION_TOLERANCE) {
            set_car_forward = true;
        } else {
            set_car_forward = false;
        }
        
        control_input_.update(set_car_forward, set_car_backward, set_car_left, set_car_right);
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

