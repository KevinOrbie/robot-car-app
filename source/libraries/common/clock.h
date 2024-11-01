/**
 * @file clock.h
 * @author Kevin Orbie
 * 
 * @brief Provides a simple, uniform time interface to the application.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <chrono>

/* Custom C++ Libraries */
// None



/* ========================== Classes ========================== */
typedef std::chrono::time_point<std::chrono::high_resolution_clock> timestamp_t;

namespace common {

inline double seconds(timestamp_t begin, timestamp_t end) {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() * 1e-6;
};

inline timestamp_t now() {
    return std::chrono::high_resolution_clock::now();
}

}  // common
