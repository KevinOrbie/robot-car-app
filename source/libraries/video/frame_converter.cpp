/**
 * @brief Defines the functions to convert between different pixelformat types.
 */

#pragma once

/* ========================== Include ========================== */
#include "frame_provider.h"

/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
#include <stdexcept>
#include <string>
#include <vector>

/* Custom C++ Libraries */
#include "common/logger.h"


namespace pixel_format {
/* ========================== Classes ========================== */
void FrameView::copyCastTo(FrameView& dst_view) {
    // Check for matching sizes.
    switch (format) {
        case PixelFormat::YUV422 : {
            if (dst_view.format == PixelFormat::YUV422P) {
                return YUV422_to_YUV422P(*this, dst_view);
            }
            break;
        }
        
        default:
            break;
    }
};

void FrameView::YUV422_to_YUV422P(FrameView &src_view, FrameView &dst_view) {
    /**
     * @note YUV 422 has 1 Cr & 1 Cb value per 2 Y values (YUYV = 2 pixels, using same U,V).
     */

    /* Sanity Check. */
    if (src_view.format != PixelFormat::YUV422) {
        LOGE("Recieved invalid input frame format: '%d'!", static_cast<int>(src_view.format));
        throw std::runtime_error("Invalid input frame format");
    }

    if (dst_view.format != PixelFormat::YUV422P) {
        LOGE("Recieved invalid output frame format: '%d'!", static_cast<int>(dst_view.format));
        throw std::runtime_error("Invalid output frame format");
    }
    
    /* Setup Source Variables. */
    uint8_t* src_frame = src_view.data;

    /* Setup Destination Variables. */
    int planar_y_base = 0;
    int planar_u_base = planar_y_base + (dst_view.size() >> 1) * dst_view.linesize[0];
    int planar_v_base = planar_u_base + (dst_view.size() >> 2) * dst_view.linesize[1];
    uint8_t* dst_frame = dst_view.data;

    /* Process two pixels / iteration. */
    for (int yidx = 0; yidx < src_view.height; yidx++) { /* Pixel coordinate in y directions. */
        for (int xidx = 0; xidx < src_view.width; xidx+=2) { /* Pixel coordinate in x direction. */
            dst_frame[planar_y_base + yidx * dst_view.linesize[0] + xidx + 0]    = src_frame[src_view.data[0] + yidx * src_view.linesize[0] + xidx * 2 + 0];  // Y1 (even pixel)
            dst_frame[planar_y_base + yidx * dst_view.linesize[0] + xidx + 1]    = src_frame[src_view.data[0] + yidx * src_view.linesize[0] + xidx * 2 + 2];  // Y2 (uneven pixel)
            dst_frame[planar_u_base + yidx * dst_view.linesize[1] + (xidx >> 1)] = src_frame[src_view.data[0] + yidx * src_view.linesize[1] + xidx * 2 + 1];  // U (use for even / uneven pixel)
            dst_frame[planar_v_base + yidx * dst_view.linesize[2] + (xidx >> 1)] = src_frame[src_view.data[0] + yidx * src_view.linesize[2] + xidx * 2 + 3];  // V (use for even / uneven pixel)
        }
    }
};

} // namespace conversions