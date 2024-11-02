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
struct Pose {
    typedef std::array<float,3> position_t;

    Pose() = default;
    Pose(float x, float y, float z): pos{x, y, z} {};

    void translate(float x, float y, float z) {
        pos[0] += x;
        pos[1] += y;
        pos[2] += z;
    };
    
    Pose invert() { return {}; };

    void setPosition();

    position_t pos;
    // TODO: rotation as eigen Rot matrix.
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
