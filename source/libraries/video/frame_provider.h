/**
 * @brief Defines Video Frame Types.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
#include <stdexcept>
#include <string>
#include <vector>


/* ========================== Classes ========================== */
enum class PixelFormat {
    EMPTY,
    YUV,
    YUV420P,
    YUV422,
    YUV422P
};


/**
 * @brief A simple OOP wrapper around a continuous piece of externally allocated memory.
 * @note Most usefull in wrapping image data from external libraries.
 */
struct FrameView {
   public:
    FrameView(uint8_t *fdata, PixelFormat fmt, int fwidth, int fheight, std::vector<int> flinesize): 
        data(fdata), format(fmt), width(fwidth), height(fheight), linesize(flinesize) {};

    int size() {
        switch (format) {
            case PixelFormat::YUV422 : return height * linesize[0];
            case PixelFormat::YUV422P: return height * linesize[0] + height * linesize[1] + height * linesize[2]; break;
            default: break;
        }
        return 0;
    }

    /**
     * @brief Cast the frame data from the this frameview to the pixelformat of dst_view, 
     * and store the casted result in the given dst_view.
     * 
     * @pre It is assumes that dst_view has already allocated sufficient space at it's data pointer.
     * @note I am not overloading the assignment operator, as I would need to do much work to follow the rule of 5.
     */
    void copyCastTo(FrameView& dst_view);

   private:
    /* Conversion Functions. */
    static void YUV422_to_YUV422P(FrameView &src_view, FrameView &dst_view);

   public:
    uint8_t *data;
    PixelFormat format;

    int width;
    int height;
    std::vector<int> linesize = {0, 0, 0};
};


/**
 * @brief A Frame, which owns it's frame data.
 */
class Frame {
   public:
    Frame() = default;

    /**
     * @brief Convert one frame to another pixel format.
     */
    Frame(Frame &frame, PixelFormat fmt) {
        Frame(frame.view(), fmt);
    }

    /**
     * @brief Create a frame for the data pointed to by the FrameView, 
     * in the specified pixel format.
     */
    Frame(FrameView new_view, PixelFormat fmt): width(new_view.width), height(new_view.height), pxl_fmt(new_view.format) {
        data.resize(size());
        FrameView current_view = view();
        new_view.copyCastTo(current_view);
    }

    /**
     * @brief Empty test initialization.
     */
    Frame(PixelFormat fmt, int width, int height): width(width), height(height) {
        int size = 0;
        switch (fmt) {
            case PixelFormat::YUV422: size = width * height * 2; break;
            case PixelFormat::YUV422P: size = width * height * 2; break;
            default: size = width * height * 3; break;
        }
        data.resize(size, 0);
    };

    /* --------------------- Functions --------------------- */
    bool empty(){
        return pxl_fmt == PixelFormat::EMPTY;
    }

    int size(){
        return width * height;
    }

    FrameView view() {
        return FrameView(data.data(), pxl_fmt, width, height, {width, width, width});
    }

   public:
    int width = 0;
    int height = 0;
    std::vector<uint8_t> data = {};
    PixelFormat pxl_fmt = PixelFormat::EMPTY;
};


/**
 * @brief An abstract interface to be implemented by classes that provide frames.
 */
class FrameProvider {
   public:
    FrameProvider() {};
    virtual Frame getFrame(double curr_time) = 0;  // Should be non-blocking?
    virtual FrameView getFrameView(double curr_time) = 0;  // Should be blocking?
    virtual void startStream() = 0;
    virtual void stopStream() = 0;
};
