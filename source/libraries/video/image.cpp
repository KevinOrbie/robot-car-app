/**
 * @brief Image class and related functionality.
 */

/* ========================== Include ========================== */
#include "image.h"

/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
#include <vector>
#include <string>
#include <stdexcept>

/* Custom C++ Libraries */
#include "common/logger.h"


/* ========================================== ImageView Class ========================================== */
ImageView::ImageView(std::vector<uint8_t*> data, std::vector<int> linesize, int width, int height, PixelFormat fmt): 
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

void ImageView::copyFrom(ImageView& view) {
    /* Verify parameters, based on given view. */
    if (height_ != view.height_ || width_ != view.width_) {
        LOGE("Invalid Argument: The given view's dimensions (%d, %d) do not match this image's dimensions (%d, %d).", view.width_, view.height_, width_, height_);
        throw std::invalid_argument("The given view's dimensions do not match this image's dimensions.");
    }

    /* Convert the given view. */
    switch (view.format_) {
        case PixelFormat::YUV422 : convertYUV422 (view, *this); break;
        case PixelFormat::YUV422P: convertYUV422P(view, *this); break;
        case PixelFormat::YUV420P: convertYUV420P(view, *this); break;

        default: 
            LOGW("No conversions supported from format '%d'!", static_cast<int>(view.format_));
            break;
    }
};


/* ============================================ Image Class ============================================ */
Image::Image(int width, int height, PixelFormat fmt)
  : width_(width), height_(height), format_(fmt), data_(getSize(fmt, width, height)) {};

Image::Image(ImageView &other_view, PixelFormat fmt)
  : Image(other_view.getWidth(), other_view.getHeight(), (fmt == PixelFormat::EMPTY) ? other_view.getFormat():fmt) {
    ImageView local_view = view();
    local_view.copyFrom(other_view);
};

ImageView Image::view() {
    std::vector<uint8_t*> data_ptrs;
    std::vector<int> linesizes;

    switch (format_) {
        case PixelFormat::YUV:
            data_ptrs = {data_.data()};
            linesizes = {3 * width_};
            break;

        case PixelFormat::YUV422:
            data_ptrs = {data_.data()};
            linesizes = {2 * width_};
            break;
        
        case PixelFormat::YUV420P:
            data_ptrs = {data_.data(), data_.data() + width_ * height_, data_.data() + width_ * height_ + (width_ >> 1) * (height_ >> 1)};
            linesizes = {width_, width_ >> 1, width_ >> 1};
            break;

        case PixelFormat::YUV422P:
            data_ptrs = {data_.data(), data_.data() + width_ * height_, data_.data() + width_ * height_ + (width_ >> 1) * height_};
            linesizes = {width_, width_ >> 1, width_ >> 1};
            break;
        
        default:
            break;
    }

    return ImageView(data_ptrs, linesizes, width_, height_, format_);
};

void Image::to(PixelFormat fmt) {
    /* Test if conversion is required. */
    if (fmt == format_) { return; }

    LOGI("Converting from %d to %d", static_cast<int>(format_), static_cast<int>(fmt));

    /* Create Image copy in requested format. */
    Image new_image = Image(width_, height_, fmt);
    ImageView new_image_view = new_image.view();
    ImageView curr_image_view = view();
    new_image_view.copyFrom(curr_image_view);

    /* Swap this frame with the new Image data. */
    std::swap(*this, new_image);

    /* NOTE: old image data leaves scope here, and is destructed as a result. */
};

void Image::zero() {
    size_t prev_data_length = data_.size();
    data_.clear();
    data_.resize(prev_data_length, 0);
};

int Image::getSize(PixelFormat fmt, int width, int height) {
    int size = 0;

    switch (fmt) {
        case PixelFormat::YUV    : size = width * height * 3; break;
        case PixelFormat::YUV422 : size = width * height * 2; break;
        case PixelFormat::YUV420P: size = (width * height * 5) << 2; break;
        case PixelFormat::YUV422P: size = width * height * 2; break;
        
        default:
            break;
    }

    return size;
};
