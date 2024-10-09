/**
 * @brief Image class and related functionality.
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


/* ========================== Classes ========================== */
enum class PixelFormat {
    EMPTY,
    YUV,
    YUV420P,
    YUV422,
    YUV422P
};

class ImageView {
   public:
    ImageView(std::vector<uint8_t*> data, std::vector<int> linesize, int width, int height, PixelFormat fmt): 
              data_(data), linesize_(linesize), width_(width), height_(height), format_(fmt) {
        /* Prepare argument expectations based on provided PixelFormat. */
        int expected_num_planes = 0;
        switch (fmt) {
            case PixelFormat::YUV:
            case PixelFormat::YUV422:
                expected_num_planes = 1;
                break;

            case PixelFormat::YUV420P:
            case PixelFormat::YUV422P:
                expected_num_planes = 3;
                break;
            
            default:
                break;
        }
        
        /* Verify parameters, based on given fmt. */
        if (static_cast<int>(data.size()) != expected_num_planes || static_cast<int>(linesize_.size()) != expected_num_planes) {
            LOGE("Invalid Argument: The given format '%d' requires %d data/linesize planes, but only %d/%d were specified.", 
                 static_cast<int>(fmt), expected_num_planes, static_cast<int>(data.size()), static_cast<int>(linesize_.size()));
            throw std::invalid_argument("Invalid number of data/linesize planes");
        }
    };

    /**
     * @brief Copy and cast the image data from this view to the other view.
     */
    void copyFrom(ImageView& view) {
        /* Verify parameters, based on given view. */
        if (height_ != view.height_ || width_ != view.width_) {
            LOGE("Invalid Argument: The given view's dimensions (%d, %d) do not match this image's dimensions (%d, %d).", view.width_, view.height_, width_, height_);
            throw std::invalid_argument("The given view's dimensions do not match this image's dimensions.");
        }

        /* Convert the given view. */
        switch (view.format_) {
            case PixelFormat::YUV422 : convertYUV422 (view, *this); break;
            case PixelFormat::YUV422P: convertYUV422P(view, *this); break;

            default: 
                LOGW("No conversions supported from format '%d'!", static_cast<int>(view.format_));
                break;
        }
    };

   private:
    PixelFormat format_ = PixelFormat::EMPTY;
    std::vector<uint8_t*> data_;
    std::vector<int> linesize_;
    int height_ = 0;
    int width_ = 0;

    /* Conversion Functions. */
    static void convertYUV422(ImageView& src, ImageView& dst);
    static void convertYUV422P(ImageView& src, ImageView& dst);
};
