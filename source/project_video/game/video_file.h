/**
 * @brief C++ wrapper around ffmpeg functionality.
 */

/* ========================== Include ========================== */
#include "frame_provider.h"

#include <stdint.h>

#include <array>
#include <vector>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


/* ========================== Classes ========================== */

/**
 * @brief Class to obtain frames from a file.
 */
class VideoFile: public FrameProvider {
   public:
    VideoFile(std::string const& filepath);
    ~VideoFile();
    Frame getFrame(double curr_time) override;
    void start() override {return;};
    void stop() override {return;};

   private:
    std::string filepath_;

    /* Container Variables */
    AVFormatContext *ptr_format_context = nullptr;  // Header information

    /* Codec Variables */
    AVCodec const *ptr_codec                = nullptr;
    AVCodecContext *ptr_codec_context       = nullptr;
    AVCodecParameters *ptr_codec_parameters = nullptr;
    int video_stream_index = -1;

    /* Stream Slice Variables */
    AVPacket *ptr_packet = nullptr;  // Encoded
    AVFrame *ptr_frame   = nullptr;  // Decoded

    /* Frame Data */
    Frame frame_data_ = {};
    double play_time_ = -1.0;
};
