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
    ImageView(std::vector<uint8_t*> data, std::vector<int> linesize, int width, int height, PixelFormat fmt);

    /**
     * @brief Copy and cast the image data from this view to the other view.
     */
    void copyFrom(ImageView& view);

    /* Getters */
    int getWidth(){ return width_; };
    int getHeight(){ return height_; };
    PixelFormat getFormat() { return format_; };

   private:
    /* Conversion Functions. */
    static void convertYUV422(ImageView& src, ImageView& dst);
    static void convertYUV420P(ImageView& src, ImageView& dst);
    static void convertYUV422P(ImageView& src, ImageView& dst);

   private:
    PixelFormat format_ = PixelFormat::EMPTY;
    std::vector<uint8_t*> data_;
    std::vector<int> linesize_;
    int height_ = 0;
    int width_ = 0;
};


class Image {
   public:
    /**
     * @brief Create empty image.
     */
    Image() = default;

    /**
     * @brief Creates and image with an uninitialized buffer.
     */
    Image(int width, int height, PixelFormat fmt);

    /**
     * @brief Creates an Image with a copy of image data from the given ImageView.
     */
    Image(ImageView &other_view, PixelFormat fmt=PixelFormat::EMPTY);

    /**
     * @brief Returns a view of the imagedata stored in this Image (only valid as long as this Image exists).
     */
    ImageView view();

    /**
     * @brief Internally changes this Image's PixelFormat to the requested format.
     */
    void to(PixelFormat fmt);

    /**
     * @brief Set all image data values to zero.
     */
    void zero();

    /* Getters */
    int getWidth(){ return width_; };
    int getHeight(){ return height_; };
    uint8_t* getData(){ return data_.data(); };
    PixelFormat getFormat() { return format_; };

   private:
    static int getSize(PixelFormat fmt, int width, int height);

    PixelFormat format_ = PixelFormat::EMPTY;
    std::vector<uint8_t> data_ = {};
    int height_ = 0;
    int width_ = 0;
};
