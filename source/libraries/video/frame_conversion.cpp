/**
 * @brief Defines the functions to convert between different pixelformat types.
 */

/* ========================== Include ========================== */
#include "frame_conversion.h"

/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
#include <stdexcept>
#include <vector>
#include <array>

/* Custom C++ Libraries */
#include "common/logger.h"


/* ========================== Classes ========================== */
// void YUV422_to_YUV(FrameView &src_view, FrameView &dst_view) {
//     /* Setup Variables */
//     uint8_t* src_frame = src_view.data[0];
//     uint8_t* dst_frame = src_view.data[0];

//     /* Process two pixels / iteration. */
//     for (int yidx = 0; yidx < dst_view.height; yidx++) { /* Pixel coordinate in y directions. */
//         for (int xidx = 0; xidx < dst_view.width; xidx += 2) { /* Pixel coordinate in x direction (left 1 of 2). */
//             int idx = (yidx * dst_view.linesize[0] + xidx) * 3;

//             /* Read even pixel. */
//             dst_frame[idx + 0] = *(src_frame + yidx * src_view.linesize[0] + xidx * 2 + 0);  // Y1
//             dst_frame[idx + 1] = *(src_frame + yidx * src_view.linesize[0] + xidx * 2 + 1);  // U (use for even / uneven pixel)
//             dst_frame[idx + 2] = *(src_frame + yidx * src_view.linesize[0] + xidx * 2 + 3);  // V (use for even / uneven pixel)
//             idx += 3;

//             /* Read uneven pixel. */
//             dst_frame[idx + 0] = *(src_frame + yidx * src_view.linesize[0] + xidx * 2 + 2);  // Y2
//             dst_frame[idx + 1] = *(src_frame + yidx * src_view.linesize[0] + xidx * 2 + 1);  // U (use for even / uneven pixel)
//             dst_frame[idx + 2] = *(src_frame + yidx * src_view.linesize[0] + xidx * 2 + 3);  // V (use for even / uneven pixel)
//         }
//     }
// }

void YUV422_to_YUV422P(uint8_t* src_data, int src_linesize, 
                       std::array<uint8_t*, 3> dst_data, std::array<int, 3> dst_linesizes,
                       int width, int height) {
    /**
     * @note YUV 422 has 1 Cr & 1 Cb value per 2 Y values (YUYV = 2 pixels, using same U,V).
     */
    /* Setup Plane Variables. */
    uint8_t* dst_planar_y_base = dst_data[0];
    uint8_t* dst_planar_u_base = dst_data[1];
    uint8_t* dst_planar_v_base = dst_data[2];

    /* Process two pixels / iteration. */
    for (int yidx = 0; yidx < height; yidx++) { /* Pixel coordinate in y directions. */
        for (int xidx = 0; xidx < width; xidx+=2) { /* Pixel coordinate in x direction (left 1 of 2). */
            *(dst_planar_y_base + yidx * dst_linesizes[0] + xidx + 0)    = *(src_data + yidx * src_linesize + xidx * 2 + 0);  // Y1 (even pixel)
            *(dst_planar_y_base + yidx * dst_linesizes[0] + xidx + 1)    = *(src_data + yidx * src_linesize + xidx * 2 + 2);  // Y2 (uneven pixel)
            *(dst_planar_u_base + yidx * dst_linesizes[1] + (xidx >> 1)) = *(src_data + yidx * src_linesize + xidx * 2 + 1);  // U (use for even / uneven pixel)
            *(dst_planar_v_base + yidx * dst_linesizes[2] + (xidx >> 1)) = *(src_data + yidx * src_linesize + xidx * 2 + 3);  // V (use for even / uneven pixel)
        }
    }
};

void YUV422P_to_YUV(std::array<uint8_t*, 3> src_data, std::array<int, 3> src_linesizes,
                    uint8_t* dst_data, int dst_linesize,
                    int width, int height) {
    /* Setup Plane Variables. */
    uint8_t* src_planar_y_base = src_data[0];
    uint8_t* src_planar_u_base = src_data[1];
    uint8_t* src_planar_v_base = src_data[2];

    /* Process 1 pixel / iteration. */
    for (int yidx = 0; yidx < height; yidx++) { /* Pixel Height Coordinate. */
        for (int xidx = 0; xidx < width; xidx++) { /* Pixel Width Coordinate. */
            *(dst_data + yidx * dst_linesize + xidx * 3 + 0) = *(src_planar_y_base + yidx * src_linesizes[0] + xidx + 0);     // Y
            *(dst_data + yidx * dst_linesize + xidx * 3 + 1) = *(src_planar_u_base + yidx * src_linesizes[1] + (xidx >> 1));  // U (use same U for even / uneven pixel)
            *(dst_data + yidx * dst_linesize + xidx * 3 + 2) = *(src_planar_v_base + yidx * src_linesizes[2] + (xidx >> 1));  // V (use same V for even / uneven pixel)
        }
    }
};

// void YUV420P_to_YUV(FrameView &src_view, FrameView &dst_view) {
//     /**
//      * @note YUV 420 has 1 Cr & 1 Cb value per 2x2 Y-block
//      */

//     /* Sanity Check. */
//     if (src_view.format != PixelFormat::YUV420P) {
//         LOGE("Recieved invalid input frame format: '%d'!", static_cast<int>(src_view.format));
//         throw std::runtime_error("Invalid input frame format");
//     }

//     if (dst_view.format != PixelFormat::YUV) {
//         LOGE("Recieved invalid output frame format: '%d'!", static_cast<int>(dst_view.format));
//         throw std::runtime_error("Invalid output frame format");
//     }

//     /* Setup Source Variables. */
//     uint8_t* src_planar_y_base = src_view.data[0];
//     uint8_t* src_planar_u_base = src_view.data[1];
//     uint8_t* src_planar_v_base = src_view.data[2];

//     /* Setup Destination Variables. */
//     uint8_t* dst_frame = dst_view.data[0];

//     /* Process 1 pixel / iteration. */
//     for (int yidx = 0; yidx < dst_view.height; yidx++) {
//         for (int xidx = 0; xidx < dst_view.width; xidx++) {
//             *(dst_frame + yidx * dst_view.linesize[0] + xidx * 3 + 0) = *(src_planar_y_base + yidx * src_view.linesize[0] + xidx);                // Y
//             *(dst_frame + yidx * dst_view.linesize[0] + xidx * 3 + 1) = *(src_planar_u_base + (yidx >> 1) * src_view.linesize[1] + (xidx >> 1));  // U (use for even / uneven pixel)
//             *(dst_frame + yidx * dst_view.linesize[0] + xidx * 3 + 2) = *(src_planar_v_base + (yidx >> 1) * src_view.linesize[2] + (xidx >> 1));  // V (use for even / uneven pixel)
//         }
//     }
// }