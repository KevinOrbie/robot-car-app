/**
 * @file utils.h
 * @author Kevin Orbie
 * 
 * @brief Specific utility functions, specifcally for the control panel.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Thirdparty C++ Libraries */
#include <eigen3/Eigen/Dense>            // Linear Algebra
#include <glm/glm.hpp>                   // GLSL Linear Algebra Library
#include <glm/gtc/matrix_transform.hpp>  // GLSL Linear Algebra Library
#include <glm/gtc/type_ptr.hpp>          // GLSL Linear Algebra Library

/* Custom C++ Libraries */
#include "common/logger.h"
#include "common/pose.h"

namespace utils {
/* ========================= Utilities ========================= */

/**
 * @brief Convert a Eigen matrix to a GLM matrix.
 */
inline glm::mat4 convert(transform_t mat) {
    glm::mat4 result;
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            result[i][j] = mat(j, i);
        }
    }
    return result;
}

}
