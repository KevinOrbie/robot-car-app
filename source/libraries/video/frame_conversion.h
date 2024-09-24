/**
 * @brief Declares functions to convert between different pixelformat types.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
#include <array>

/* Custom C++ Libraries */
// None


/* ========================== Classes ========================== */
void YUV422_to_YUV422P(uint8_t* src_data, int src_linesize, 
                       std::array<uint8_t*, 3> dst_data, std::array<int, 3> dst_linesizes,
                       int width, int height);

void YUV422P_to_YUV(std::array<uint8_t*, 3> src_data, std::array<int, 3> src_linesizes,
                    uint8_t* dst_data, int dst_linesize,
                    int width, int height);
