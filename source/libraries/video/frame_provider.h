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
#include <stdexcept>

/* Custom C++ Libraries */
#include "common/logger.h"
#include "image.h"


/* ========================== Classes ========================== */
struct Frame {
    Image image;
};

class FrameProvider {
   public:
    /**
     * @note This virtual destructor is needed to allow derived classes to be polymophically destructed.
     * @link https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
     */
    virtual ~FrameProvider(){}; 

    virtual Frame getFrame(double curr_time, PixelFormat requested_format) = 0;
    virtual void startStream() = 0;
    virtual void stopStream() = 0;
};
