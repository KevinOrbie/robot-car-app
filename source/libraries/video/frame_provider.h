/**
 * @brief C++ wrapper around ffmpeg functionality.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
#include <vector>
#include <string>


/* ========================== Classes ========================== */
struct Frame {
    int width = 0;
    int height = 0;
    int channels = 0;
    std::vector<uint8_t> data;
};

class FrameProvider {
   public:
    FrameProvider() {};
    virtual Frame getFrame(double curr_time) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};
