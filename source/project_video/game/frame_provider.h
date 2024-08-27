/**
 * @brief C++ wrapper around ffmpeg functionality.
 */

/* ========================== Include ========================== */
#include <stdint.h>

#include <vector>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


/* ========================== Classes ========================== */
struct Frame {
    int width = 0;
    int height = 0;
    int channels = 0;
    std::vector<uint8_t> data;
};

class FrameProvider {
   public:
    FrameProvider() {};
    virtual Frame getFrame() = 0;
};

class VideoFile: public FrameProvider {
   public:
    VideoFile(std::string const& filepath);
    ~VideoFile();
    Frame getFrame() override;

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
};

// TODO: Display 1 frame
// TODO: Send Y_CrCb (YUV 4:2:0 12bpp / YUV420P) image to GPU as RGB texture, and update the shader to calculate the RGB values itself.

// TODO: Display total video
// TODO: Fix replay rate (using PTS timestamps)

// TODO: Create new FrameProvider class called VideoCam
