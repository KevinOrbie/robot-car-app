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
#include <array>

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


class Pose {
   public:
    Pose() = default;
    
    Pose(position_t position): pos_(position) {};
    Pose(rotation_t rotation): rot_(rotation) {};
    Pose(position_t position, rotation_t rotation): pos_(position), rot_(rotation) {};
    Pose(double x, double y, double z): pos_(x, y, z) {};

    void rotate() {};
    void translate(double x, double y, double z) {
        pos_[0] += x;
        pos_[1] += y;
        pos_[2] += z;
    };

    void setPosition(position_t position) { pos_ = position; };
    position_t getPosition() { return pos_; };
    double x() { return pos_[0]; };
    double y() { return pos_[1]; };
    double z() { return pos_[2]; };

    Pose inverse() { return {}; };
    transform_t toMatrix() { 
        transform_t transform = transform_t::Identity();
        transform.block(0,0,3,3) = rot_;
        transform.block(0,3,3,1) = pos_;
        return transform; 
    };

    Pose operator*(const Pose &other) { return {}; };
    position_t operator*(const position_t &other) { return {}; };

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
