/**
 * @file video_transmitter.cpp
 * @author Kevin Orbie
 * 
 * @brief C++ wrapper around ffmpeg streaming functionality.
 * @link based on: https://stackoverflow.com/questions/48564047/c-using-ffmpeg-encode-and-udp-with-a-webcam
 */

/* ============================ Includes ============================ */
#include "video_transmitter.h"

/* Standard C++ Libraries */
#include <stdexcept>
#include <string>

/* Custom C++ Libraries */
#include "common/logger.h"
#include "video/frame_conversion.h"


/* ============================ Classes ============================ */
VideoTransmitter::VideoTransmitter(std::string const& address, FrameProvider *frame_provider): 
    address_(address), frame_provider_(frame_provider) {
    avformat_network_init();
    // av_log_set_level(AV_LOG_DEBUG);
    av_log_set_level(AV_LOG_QUIET);

    /* ---------------- Setup Container Context ----------------- */
    /* This context is used during the muxing operation. */

    // udp: mpegts
    // tcp: mpegts
    // rtp: rtp
    // rtmp: flv
    // rtsp: rtsp
    int res = avformat_alloc_output_context2(&ptr_format_context, NULL, "mpegts", address.c_str());
    if (res < 0) {
        LOGE("Issue while allocating memory for output Format Context: %d", res);
        throw std::runtime_error("Failed to allocate memory for output Format Context");
    } else if (!ptr_format_context) {
        LOGE("Could not allocate memory for output Format Context.");
        throw std::runtime_error("Failed to allocate memory for output Format Context");
    }

    /* ---------------------- Setup CODEC ----------------------- */
    /* Setup Encoder. */
    ptr_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!ptr_codec) {
        LOGE("Could not find encoder.");
        throw std::runtime_error("Failed to find encoder");
    }

    /* Allocate video CODEC Context. */
    ptr_codec_context = avcodec_alloc_context3(ptr_codec);
    if (!ptr_codec_context) {
        LOGE("Failed to allocated memory for AVCodecContext.");
        throw std::runtime_error("Failed to allocated memory for AVCodecContext");
    }

    /* Fill the CODEC context. */
    ptr_codec_context->codec_id = AV_CODEC_ID_H264;
    ptr_codec_context->bit_rate = 1000000;  // 1 Mbps
    ptr_codec_context->width = 2560;
    ptr_codec_context->height = 720;
    ptr_codec_context->time_base = AVRational{1, 30};
    ptr_codec_context->framerate = AVRational{30, 1};
    ptr_codec_context->gop_size = 12;                   // How many frames between full frames sent.
    ptr_codec_context->pix_fmt = AV_PIX_FMT_YUV422P; // AV_PIX_FMT_YUV420P;

    /* Setup Options. */
    if (ptr_format_context->flags & AVFMT_GLOBALHEADER) {
        ptr_codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    /* ---------------------- Setup Stream ---------------------- */
    /* Create Stream. */
    ptr_stream = avformat_new_stream(ptr_format_context, ptr_codec);
    if (!ptr_stream) {
        LOGE("Could not allocate output stream.");
        throw std::runtime_error("Failed to allocate output stream");
    }

    /* Fill in stream parameters. */
    ptr_stream->id = ptr_format_context->nb_streams - 1;
    ptr_stream->time_base = ptr_codec_context->time_base;

    /* Setup the stream context based on codec paramters. */
    if (avcodec_parameters_from_context(ptr_stream->codecpar, ptr_codec_context) < 0) {
        LOGE("Could not copy codec parameters.");
        throw std::runtime_error("Failed to copy codec parameters");
    }

    /* Print information about Stream Format. */
    av_dump_format(ptr_format_context, 0, address.c_str(), 1);

    /* Open / initialize the output container file (if needed). */
    if (!(ptr_format_context->oformat->flags & AVFMT_NOFILE)) {
        // av_dict_set(&ptr_open_container_opts, "pkt_size", "1200", 0); // Set packet size to 1200 bytes
        av_dict_set(&ptr_open_container_opts, "buffer_size", "1048576", 0);  // Optional but might help
        av_dict_set(&ptr_open_container_opts, "max_delay", "0", 0);
        av_dict_set(&ptr_open_container_opts, "flags", "low_delay", 0);

        /* File required by the container format. */
        if (avio_open2(&ptr_format_context->pb, address.c_str(), AVIO_FLAG_WRITE, NULL, &ptr_open_container_opts) < 0) {
            LOGE("Could not open output file '%s'.", address.c_str());
            throw std::runtime_error("Failed to open output file");
        }
    }

    /* Allocate Stream data & write the stream header to an output media file. */
    if (avformat_write_header(ptr_format_context, NULL) != 0) {
        LOGE("Failed to connect to '%s'.", address.c_str());
        throw std::runtime_error("Failed to connect to network.");
    }

    /* ----------------------- Open CODEC ----------------------- */
    if (ptr_codec_context->codec_id == AV_CODEC_ID_H264) {
        // av_dict_set(&ptr_codec_opts, "profile", "high", 0);  // Only supported with YUV420P
        av_dict_set(&ptr_codec_opts, "profile", "high422", 0);
        av_dict_set(&ptr_codec_opts, "preset", "veryfast", 0);
        av_dict_set(&ptr_codec_opts, "tune", "zerolatency", 0);
        av_dict_set(&ptr_codec_opts, "x264-params", "keyint=30", 0);
    }

    /* Initialize the AVCodecContext to use the given AVCodec. */
    if (avcodec_open2(ptr_codec_context, ptr_codec, &ptr_codec_opts) < 0) {
        LOGE("Failed to open CODEC through avcodec_open2.");
        throw std::runtime_error("Failed to open CODEC through avcodec_open2");
    }

    /* ---------------- Allocate Packet / Frame ----------------- */
    ptr_frame = av_frame_alloc();
    if (!ptr_frame) {
        LOGE("Failed to allocate memory for AVFrame.");
        throw std::runtime_error("Failed to allocate memory for AVFrame.");
    }

    ptr_frame->format = ptr_codec_context->pix_fmt;
    ptr_frame->width = ptr_codec_context->width;
    ptr_frame->height = ptr_codec_context->height;

    /* Allocate frame data (data buffer). */
    if (av_frame_get_buffer(ptr_frame, 0) < 0) {
        LOGE("Failed to allocate memory for AVFrame data.");
        throw std::runtime_error("Failed to allocate memory for AVFrame data");
    }
    av_frame_make_writable(ptr_frame);

    /* Allocate Packet */
    ptr_packet = av_packet_alloc();
    if (!ptr_packet) {
        LOGE("Failed to allocate memory for AVPacket.");
        throw std::runtime_error("Failed to allocate memory for AVPacket");
    }

    auto start_time_ = std::chrono::steady_clock::now();
}

VideoTransmitter::~VideoTransmitter() {
    av_write_trailer(ptr_format_context);

    av_packet_free(&ptr_packet);
    av_frame_free(&ptr_frame);
    avformat_close_input(&ptr_format_context);
    avio_close(ptr_format_context->pb);
    avformat_free_context(ptr_format_context);
    avcodec_free_context(&ptr_codec_context);
    av_dict_free(&ptr_codec_opts);
    av_dict_free(&ptr_open_container_opts);
}

void VideoTransmitter::iteration() {
    if (frame_provider_) {
        /* Video from frame provider (YUV422). */
        Frame frame = frame_provider_->getFrame(INFINITY); // Always get the latest frame.
        send(frame);

    } else {
        /* Testing video. */
        // NOTE: YUV (0,0,0) is roughly RGB (0, 136, 0)
        Frame frame = {2560, 720, 2, {}}; // YUV422
        frame.data.resize(frame.width * frame.height * 2, 0);

        static int counter = 0;
        /* Fill in a box. */
        for (int yidx = 0; yidx < frame.height; yidx++) {
            for (int xidx = (counter % frame.width); xidx < (counter + 100) % frame.width; xidx++) {
                int idx = (yidx * frame.width * 2 + xidx * 2);

                /* Only fill the y plane. */
                frame.data[idx] = 255;
            }
        }
        counter += 20;

        send(frame);

        /* Fix to 30 FPS. */
        std::this_thread::sleep_until(start_time_ + std::chrono::milliseconds(33));
        start_time_ = std::chrono::steady_clock::now();
    }
}

/**
 * @brief Send the given frame over the network.
 * 
 * @note Frames are sent as packets, and as a result, not every call to this 
 * function will directly send the output.
 * @link https://www.ffmpeg.org/doxygen/trunk/remux_8c-example.html#a48
 */
void VideoTransmitter::send(Frame &frame) {
    /* Add data to frame. */
    ptr_frame->pts = frame_pts;

    /* Copy YUV422 to YUV422P. */
    YUV422_to_YUV422P(
        frame.data.data(), frame.width * 2, 
        {ptr_frame->data[0], ptr_frame->data[1], ptr_frame->data[2]},
        {ptr_frame->linesize[0], ptr_frame->linesize[1], ptr_frame->linesize[2]},
        frame.width, frame.height
    );

    /* Copy data from custom Frame to Libav frame (both YUV422P). */
    // int base_u_idx = frame.height * frame.width;
    // int base_v_idx = (frame.height * frame.width * 3) >> 1;

    // for (int yidx = 0; yidx < frame.height; yidx++) {
    //     for (int xidx = 0; xidx < frame.width; xidx++) {
    //         /* Copy Y value. */
    //         int src_y_idx = yidx * frame.width + xidx;
    //         int dst_y_offset = yidx * ptr_frame->linesize[0] + xidx;
    //         *(ptr_frame->data[0] + dst_y_offset) = frame.data[src_y_idx];

    //         /* Copy U value. */
    //         int src_u_idx = (yidx * frame.width + xidx) >> 1;
    //         int dst_u_offset = yidx * ptr_frame->linesize[1] + (xidx >> 1);
    //         *(ptr_frame->data[1] + dst_u_offset) = frame.data[base_u_idx + src_u_idx];

    //         /* Copy V value. */
    //         int src_v_idx = (yidx * frame.width + xidx) >> 1;
    //         int dst_v_offset = yidx * ptr_frame->linesize[2] + (xidx >> 1);
    //         uint8_t value = frame.data[base_v_idx + src_v_idx];
    //         *(ptr_frame->data[2] + dst_v_offset) = value;  // Segfault here.
    //     }
    // }

    /* Send a frame to the encoder. */
    if (avcodec_send_frame(ptr_codec_context, ptr_frame) < 0) {
        LOGE("Issue encoding frame.");
        throw std::runtime_error("Failed to encode frame");
    }

    int ret = 0;
    do { /* Send all available packets. */
        av_packet_unref(ptr_packet);

        /* Recieve a packet from the encoder. */
        ret = avcodec_receive_packet(ptr_codec_context, ptr_packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            LOGE("Issue during recieving encoded packet: %d", ret);
            throw std::runtime_error("Failed to recieve encoded packet");
        }

        ret = av_interleaved_write_frame(ptr_format_context, ptr_packet);
        if (ret < 0) {
            LOGE("Issue writing packet to stream");
            throw std::runtime_error("Failed to write a packet to stream");
        }
    } while (ret >= 0);

    frame_pts++;
}