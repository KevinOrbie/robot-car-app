/**
 * @file video_reciever.h
 * @author Kevin Orbie
 * 
 * @brief C++ wrapper around ffmpeg functionality.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
#include <vector>
#include <string>
#include <mutex>

/* Third Party C++ Libraries */
extern "C" { // ffmpeg
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

/* Custom C++ Libraries */
#include "common/looper.h"
#include "frame_provider.h"


/* ========================== Classes ========================== */

/**
 * @brief Class to obtain frames from a file.
 */
class VideoReciever: public Looper, public FrameProvider {
   public:
    VideoReciever(std::string const& address=std::string("udp://127.0.0.1:8999"));
    ~VideoReciever();

    void iteration() override;
    void recieve(); // Blocking

    Frame getFrame(double curr_time) override;
    void startStream() override {};
    void stopStream() override {};

   private:
    std::string address_;
    FrameProvider *frame_provider_ = nullptr;

    /* Container Variables (for muxing) */
    AVDictionary    *ptr_open_container_opts = nullptr;
    AVFormatContext *ptr_format_context      = nullptr;  // Header information
    int              video_stream_index      = -1;

    /* Codec Variables (for encoding) */
    AVCodecParameters *ptr_codec_parameters = nullptr;
    AVCodecContext    *ptr_codec_context    = nullptr;
    AVCodec const     *ptr_codec            = nullptr;

    /* Stream Slice Variables */
    AVPacket *ptr_packet = nullptr;  // Encoded
    AVFrame  *ptr_frame  = nullptr;  // Decoded

    /* Frame Data */
    Frame  frame_data_ = {};
    int    frame_pts   = 0;
    std::mutex frame_data_mutex_;
};
