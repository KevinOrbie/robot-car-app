/**
 * @brief Implements conversions between different image pixel formats.
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


/* ========================== Classes ========================== */
void ImageView::convertYUV422(ImageView& src, ImageView& dst) {
    /**
     * @note YUV 422 has 1 Cr & 1 Cb value per 2 Y values (YUYV = 2 pixels, using same U,V).
     */
    int width = src.width_;
    int height = src.height_;

    /* Validate Arguments. */
    if (src.format_ != PixelFormat::YUV422) {
        LOGE("Invalid Argument: Expected src to have the YUV422P pixel format.");
        throw std::invalid_argument("The given src does not have the YUV422P pixel format.");
    }

    /* Convert to Destiation Format. */
    switch (dst.format_) {
        /* ------------------------ YUV422 to YUV422 ------------------------- */
        case PixelFormat::YUV422: {
            /* Process 1 pixel / iteration. */
            for (int yidx = 0; yidx < height; yidx++) { /* Image pixel coordinate system. */
                for (int xidx = 0; xidx < width * 2; xidx++) { /* Byte coordinate system. */
                    int src_offset = (yidx * src.linesize_[0] + xidx);
                    int dst_offset = (yidx * dst.linesize_[0] + xidx);
                    *(dst.data_[0] + dst_offset) = *(src.data_[0] + src_offset);
                }
            }

            break;
        }

        /* -------------------------- YUV422 to YUV -------------------------- */
        case PixelFormat::YUV: {
            /* Process 2 pixel / iteration. */
            for (int yidx = 0; yidx < height; yidx++) { /* Pixel Height Coordinate. */
                for (int xidx = 0; xidx < width; xidx+=2) { /* Pixel Width Coordinate. */
                    /* Converting even pixel. */
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 0) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 0);  // Y1
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 1) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 1);  // U (use same U for even / uneven pixel)
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 2) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 3);  // V (use same V for even / uneven pixel)
                
                    /* Converting uneven pixel. */
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 3) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 2);  // Y2
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 4) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 1);  // U (use same U for even / uneven pixel)
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 5) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 3);  // V (use same V for even / uneven pixel)
                }
            }
            break;
        }

        /* ------------------------ YUV422 to YUV422P ------------------------ */
        case PixelFormat::YUV422P: {
            /* Setup Plane Variables. */
            uint8_t* dst_planar_y_base = dst.data_[0];
            uint8_t* dst_planar_u_base = dst.data_[1];
            uint8_t* dst_planar_v_base = dst.data_[2];

            /* Process two pixels / iteration. */
            for (int yidx = 0; yidx < height; yidx++) { /* Pixel coordinate in y directions. */
                for (int xidx = 0; xidx < width; xidx+=2) { /* Pixel coordinate in x direction (left 1 of 2). */
                    *(dst_planar_y_base + yidx * dst.linesize_[0] + xidx + 0)    = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 0);  // Y1 (even pixel)
                    *(dst_planar_y_base + yidx * dst.linesize_[0] + xidx + 1)    = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 2);  // Y2 (uneven pixel)
                    *(dst_planar_u_base + yidx * dst.linesize_[1] + (xidx >> 1)) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 1);  // U (use for even / uneven pixel)
                    *(dst_planar_v_base + yidx * dst.linesize_[2] + (xidx >> 1)) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 3);  // V (use for even / uneven pixel)
                }
            }
            
            break;
        }

        /* -------------------------- YUV422 to GREY -------------------------- */
        case PixelFormat::GREY: {
            /* Process 2 pixel / iteration. */
            for (int yidx = 0; yidx < height; yidx++) { /* Pixel Height Coordinate. */
                for (int xidx = 0; xidx < width; xidx+=2) { /* Pixel Width Coordinate. */
                    /* Converting even pixel. */
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 0) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 0);  // Y1
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 1) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 1);  // U (use same U for even / uneven pixel)
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 2) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 3);  // V (use same V for even / uneven pixel)
                
                    /* Converting uneven pixel. */
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 3) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 2);  // Y2
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 4) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 1);  // U (use same U for even / uneven pixel)
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 5) = *(src.data_[0] + yidx * src.linesize_[0] + xidx * 2 + 3);  // V (use same V for even / uneven pixel)
                }
            }
            break;
        }
        
        default:
            LOGW("Conversion from format '%d' to '%d' is not supported!", static_cast<int>(src.format_), static_cast<int>(dst.format_));
            break;
    }
};

void ImageView::convertYUV420P(ImageView& src, ImageView& dst) {
    int width = src.width_;
    int height = src.height_;

    /* Validate Arguments. */
    if (src.format_ != PixelFormat::YUV420P) {
        LOGE("Invalid Argument: Expected src to have the YUV420P pixel format.");
        throw std::invalid_argument("The given src does not have the YUV420P pixel format.");
    }

    /* Convert to Destiation Format. */
    switch (dst.format_) {
        /* ------------------------ YUV420P to YUV ------------------------ */
        case PixelFormat::YUV: {
            /* Setup Plane Variables. */
            uint8_t* src_planar_y_base = src.data_[0];
            uint8_t* src_planar_u_base = src.data_[1];
            uint8_t* src_planar_v_base = src.data_[2];

            /* Process 1 pixel / iteration. */
            for (int yidx = 0; yidx < height; yidx++) { /* Pixel Height Coordinate. */
                for (int xidx = 0; xidx < width; xidx++) { /* Pixel Width Coordinate. */
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 0) = *(src_planar_y_base + yidx * src.linesize_[0] + xidx + 0);            // Y
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 1) = *(src_planar_u_base + (yidx >> 1) * src.linesize_[1] + (xidx >> 1));  // U (use same U for even / uneven pixel)
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 2) = *(src_planar_v_base + (yidx >> 1) * src.linesize_[2] + (xidx >> 1));  // V (use same V for even / uneven pixel)
                }
            }
            break;
        }
        
        default:
            LOGW("Conversion from format '%d' to '%d' is not supported!", static_cast<int>(src.format_), static_cast<int>(dst.format_));
            break;
    }
};

void ImageView::convertYUV422P(ImageView& src, ImageView& dst) {
    int width = src.width_;
    int height = src.height_;

    /* Validate Arguments. */
    if (src.format_ != PixelFormat::YUV422P) {
        LOGE("Invalid Argument: Expected src to have the YUV422P pixel format.");
        throw std::invalid_argument("The given src does not have the YUV422P pixel format.");
    }

    /* Convert to Destiation Format. */
    switch (dst.format_) {
        /* ---------------------- YUV422P to YUV422P ---------------------- */
        case PixelFormat::YUV422P: {
            /* Setup Plane Variables. */
            uint8_t* src_planar_y_base = src.data_[0];
            uint8_t* src_planar_u_base = src.data_[1];
            uint8_t* src_planar_v_base = src.data_[2];

            uint8_t* dst_planar_y_base = dst.data_[0];
            uint8_t* dst_planar_u_base = dst.data_[1];
            uint8_t* dst_planar_v_base = dst.data_[2];

            /* Copy over ~1 pixel / iteration. */
            for (int yidx = 0; yidx < height; yidx++) { /* Pixel Height Coordinate. */
                for (int xidx = 0; xidx < width; xidx++) { /* Pixel Width Coordinate. */

                    /* Copy over Y pixel. */
                    *(dst_planar_y_base + yidx * dst.linesize_[0] + xidx + 0) = *(src_planar_y_base + yidx * src.linesize_[0] + xidx + 0);

                    /* Copy over U & V pixels (once for every two pixels). */
                    if (xidx % 2 == 0) {
                        *(dst_planar_u_base + yidx * dst.linesize_[1] + (xidx >> 1)) = *(src_planar_u_base + yidx * src.linesize_[1] + (xidx >> 1));  // U (use same U for even / uneven pixel)
                        uint8_t val = *(src_planar_v_base + yidx * src.linesize_[2] + (xidx >> 1));
                        *(dst_planar_v_base + yidx * dst.linesize_[2] + (xidx >> 1)) = val;  // V (use same V for even / uneven pixel)
                    }
                }
            }
            break;
        }

        /* ------------------------ YUV422P to YUV ------------------------ */
        case PixelFormat::YUV: {
            /* Setup Plane Variables. */
            uint8_t* src_planar_y_base = src.data_[0];
            uint8_t* src_planar_u_base = src.data_[1];
            uint8_t* src_planar_v_base = src.data_[2];

            /* Process 1 pixel / iteration. */
            for (int yidx = 0; yidx < height; yidx++) { /* Pixel Height Coordinate. */
                for (int xidx = 0; xidx < width; xidx++) { /* Pixel Width Coordinate. */
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 0) = *(src_planar_y_base + yidx * src.linesize_[0] + xidx + 0);     // Y
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 1) = *(src_planar_u_base + yidx * src.linesize_[1] + (xidx >> 1));  // U (use same U for even / uneven pixel)
                    *(dst.data_[0] + yidx * dst.linesize_[0] + xidx * 3 + 2) = *(src_planar_v_base + yidx * src.linesize_[2] + (xidx >> 1));  // V (use same V for even / uneven pixel)
                }
            }
            break;
        }
        
        default:
            LOGW("Conversion from format '%d' to '%d' is not supported!", static_cast<int>(src.format_), static_cast<int>(dst.format_));
            break;
    }
};
