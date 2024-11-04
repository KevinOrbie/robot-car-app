/**
 * @file pose.h
 * @author Kevin Orbie
 * 
 * @brief Declares pose related classes and functionality.
 * @note Pose here refers to the combination of a 3D position & orientation.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Thirdparty C++ Libraries */
#include <eigen3/Eigen/Dense>

/* Custom C++ Libraries */
#include "logger.h"
#include "clock.h"


/* ========================== Classes ========================== */
typedef Eigen::Vector3d position_t;
typedef Eigen::Matrix3d rotation_t;
typedef Eigen::Matrix4d transform_t;

typedef Eigen::Vector3d velocity_t;
typedef Eigen::Vector3d acceleration_t;
typedef Eigen::Vector3d angular_velocity_t;
typedef Eigen::Vector3d angular_acceleration_t;


/**
 * @brief Represents a pose / spatial transform.
 * 
 * @details A Pose_AB represents the following position & rotation:
 *   > Position_AB_A: Position of Frame B, relative to Frame A, expressed in frame A.
 *   > Rotation_AB:   Rotation of Frame B relative to Frame A.
 */
class Pose {
   public:
    Pose() = default;
    
    Pose(position_t position): pos_(position) {};
    Pose(rotation_t rotation): rot_(rotation) {};
    Pose(position_t position, rotation_t rotation): pos_(position), rot_(rotation) {};
    Pose(double x, double y, double z): pos_(x, y, z) {};
    
    void setPosition(position_t position) { pos_ = position; };
    position_t getPosition() const { return pos_; };
    double x() { return pos_[0]; };
    double y() { return pos_[1]; };
    double z() { return pos_[2]; };
    void translate(Eigen::Vector3d offset) {
        pos_ += offset;
    };

    void setRotation(rotation_t rotation) { rot_ = rotation; };
    rotation_t getRotation() const { return rot_; };
    double xrot() {
        Eigen::Vector3d euler_angles = rot_.eulerAngles(0, 1, 2);
        return euler_angles[0];
    };
    double yrot() {
        Eigen::Vector3d euler_angles = rot_.eulerAngles(0, 1, 2);
        return euler_angles[1];
    };
    double zrot() {
        Eigen::Vector3d euler_angles = rot_.eulerAngles(0, 1, 2);
        return euler_angles[2];
    };
    /**
     * @brief Extrinsic XYZ Euler angles Rotation around the axis of this Pose's reference Frame.
     * 
     * @post The Pose's position remains unchanged.
     * 
     * @param roll:  Rotation around the X-axis of the current reference Frame.
     * @param pitch: Rotation around the Y-axis of the current reference Frame.
     * @param yaw:   Rotation around the Z-axis of the current reference Frame.
     */
    void rotate(double roll, double pitch, double yaw) {
        rotation_t rotation = (Eigen::AngleAxisd(roll , Eigen::Vector3d::UnitX())
                             * Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitY())
                             * Eigen::AngleAxisd(yaw  , Eigen::Vector3d::UnitZ())).toRotationMatrix();
        rot_ = rotation * rot_;
    };
    void rotate(Eigen::Vector3d euler_angles) {
        rotate(euler_angles[0], euler_angles[1], euler_angles[2]);
    };

    /**
     * @brief Get the unit axis of this Pose's Frame, expressed in reference frame coordinates.
     */
    Eigen::Vector3d UnitX() { 
        return rot_.block(0,0,3,1); 
    };
    Eigen::Vector3d UnitY() { return rot_.block(0,1,3,1); };
    Eigen::Vector3d UnitZ() { return rot_.block(0,2,3,1); };

    /**
     * @brief Returns the inverse pose.
     * 
     * @details Inverting Pose_AB results in Pose_BA:
     *   > Position_AB_A --> Rotation_BA * Position_BA_A = Position_BA_B
     *   > Rotation_AB   --> Rotation_BA
     */
    Pose inverse() {
        rotation_t inv_rot = rot_.transpose();
        position_t inv_pos = -(inv_rot * pos_);
        return Pose(inv_pos, inv_rot); 
    };
    
    transform_t toMatrix() { 
        transform_t transform = transform_t::Identity();
        transform.block(0,0,3,3) = rot_;
        transform.block(0,3,3,1) = pos_;
        return transform; 
    };

    /**
     * @brief Pose_AB * Pose_BC = Pose_AC
     *   > Position_AC_A = Rotation_AC * Position_AB_A + Rotation_CB * Position_BC_B
     *   > Rotation_AC   = Rotation_AB * Rotation_BC
     */
    Pose operator*(const Pose &other) { 
        rotation_t new_rot = getRotation() * other.getRotation();
        position_t new_pos = new_rot * getPosition() + other.getRotation().transpose() * other.getPosition();
        return Pose(new_pos, new_rot); 
    };

    /**
     * @brief Pose_AB * Posisiton_BC_B = Position_AC_A
     */
    Eigen::Vector3d operator*(const Eigen::Vector3d &other) {
        return getPosition() + getRotation() * other; 
    };

    /**
     * @brief Pose_AB + Posisiton_BC_A = Pose_AC
     */
    Pose operator+(const position_t &other) { 
        return Pose(getPosition() + other, getRotation()); 
    };

    void print() {
        LOGI("Pose: [%f, %f, %f], [%f, %f, %f]", x(), y(), z(), xrot(), yrot(), zrot());
    }

   private:
    position_t pos_;
    rotation_t rot_ = rotation_t::Identity();
};


class PoseProvider {
   public:
    /**
     * @note This virtual destructor is needed to allow derived classes to be polymophically destructed.
     * @link https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
     */
    virtual ~PoseProvider(){}; 

    /* Rule of Five. */
    PoseProvider()                                       = default;
    PoseProvider(PoseProvider && other)                  = default;
    PoseProvider(const PoseProvider& other)              = default;
    PoseProvider& operator=(PoseProvider && other)       = default;
    PoseProvider& operator=(const PoseProvider& other)   = default;

    /* Interface */
    virtual Pose getPose(timestamp_t request_timestamp) = 0;
};
