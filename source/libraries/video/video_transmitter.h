/**
 * @file video_transmitter.h
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
class VideoTransmitter: public Looper {
   public:
    VideoTransmitter(std::string const& address=std::string("udp://127.0.0.1:8999"), FrameProvider *frame_provider=nullptr);
    ~VideoTransmitter();

    void start() override;
    void iteration() override;
    void send(Frame &frame);

   private:
    std::string address_;
    FrameProvider *frame_provider_ = nullptr;

    /* Container Variables (for muxing) */
    AVFormatContext *ptr_format_context = nullptr;  // Header information
    AVDictionary    *ptr_open_container_opts = nullptr;

    /* Codec Variables (for encoding) */
    AVCodecContext *ptr_codec_context   = nullptr;
    AVDictionary   *ptr_codec_opts      = nullptr;
    AVCodec const  *ptr_codec           = nullptr;

    /* Stream */
    AVStream *ptr_stream;

    /* Stream Slice Variables */
    AVPacket *ptr_packet = nullptr;  // Encoded
    AVFrame  *ptr_frame  = nullptr;  // Decoded

    /* Frame Data */
    Frame  frame_data_ = {};
    double play_time_  = -1.0;
    int    frame_pts   = 0;

    /* Timing */
    std::chrono::_V2::steady_clock::time_point start_time_;
};
