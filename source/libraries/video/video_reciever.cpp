/**
 * @file video_reciever.cpp
 * @author Kevin Orbie
 * 
 * @brief C++ wrapper around ffmpeg streaming functionality.
 * @link based on: https://stackoverflow.com/questions/48564047/c-using-ffmpeg-encode-and-udp-with-a-webcam
 */

/* ============================ Includes ============================ */
#include "video_reciever.h"

/* Standard C Libraries */
#include <unistd.h>  // gettid()

/* Standard C++ Libraries */
#include <stdexcept>
#include <string>
#include <mutex>

/* Custom C++ Libraries */
#include "common/logger.h"
#include "video/image.h"


/* ============================ Classes ============================ */
VideoReciever::VideoReciever(std::string const& address): address_(address){
    avformat_network_init();
    // av_log_set_level(AV_LOG_DEBUG);
    av_log_set_level(AV_LOG_QUIET);

    /* ---------------- Read Container Context ----------------- */
    /* This context is used during the muxing operation. */
    LOGI("Waiting for video data from: '%s'", address.c_str());

    ptr_format_context = avformat_alloc_context();
    if (!ptr_format_context) {
        LOGE("Could not allocate memory for a Format Context.");
        throw std::runtime_error("Failed to allocate memory for a Format Context");
    }

    /* Get Header Information */
    // udp: mpegts (av_find_input_format("mpegts"))
    // tcp: mpegts
    // rtp: rtp
    // rtmp: flv
    // rtsp: rtsp
    int res = avformat_open_input(&ptr_format_context, address.c_str(), NULL, NULL);
    if (res < 0) {
        LOGE("Issue while opening the input stream: %d", res);
        throw std::runtime_error("Failed to open the input stream");
    }

    LOGI("format %s, duration %ld us, bit_rate %ld", ptr_format_context->iformat->name, ptr_format_context->duration, ptr_format_context->bit_rate);

    /* Get Stream Information */
    if (avformat_find_stream_info(ptr_format_context,  NULL) < 0) {
        LOGE("Could not get the stream info");
        throw std::runtime_error("Failed get the stream info");
    }

    /* ---------------------- Setup CODEC ----------------------- */
    /* Loop over all streams */
    for (int i = 0; i < ptr_format_context->nb_streams; i++) {
        /* Find stream's CODEC parameters. */
        AVCodecParameters *ptr_local_codec_params =  NULL;
        ptr_local_codec_params = ptr_format_context->streams[i]->codecpar;

        LOGI(" ======== Stream %d ========", i);
        LOGI("AVStream->time_base before open coded %d/%d", ptr_format_context->streams[i]->time_base.num, ptr_format_context->streams[i]->time_base.den);
        LOGI("AVStream->r_frame_rate before open coded %d/%d", ptr_format_context->streams[i]->r_frame_rate.num, ptr_format_context->streams[i]->r_frame_rate.den);
        LOGI("AVStream->start_time %ld" PRId64, ptr_format_context->streams[i]->start_time);
        LOGI("AVStream->duration %ld" PRId64, ptr_format_context->streams[i]->duration);

        /* Find CODEC decoder for the current stream. */
        AVCodec const *ptr_local_codec = NULL;
        ptr_local_codec = avcodec_find_decoder(ptr_local_codec_params->codec_id);

        if (ptr_local_codec==NULL) {
            LOGE("Unsupported CODEC!");
            throw std::runtime_error("Unsupported CODEC");
        }

        /* Process Video Streams. */
        if (ptr_local_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                /* Use first video CODEC as main codec. */
                video_stream_index = i;
                ptr_codec = ptr_local_codec;
                ptr_codec_parameters = ptr_local_codec_params;
            }
            LOGI("Video Codec: resolution %d x %d", ptr_local_codec_params->width, ptr_local_codec_params->height);
        }
        
        /* Ignore the Audio Streams. */
        // if (ptr_local_codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
        //     LOGI("Audio Codec: %d channels, sample rate %d", ptr_local_codec_params->channels, ptr_local_codec_params->sample_rate);
        // }

        LOGI("\tCodec %s ID %d bit_rate %ld", ptr_local_codec->name, ptr_local_codec->id, ptr_local_codec_params->bit_rate);
    }

    /* Verify a video stream was found. */
    if (video_stream_index == -1) {
        LOGE("Address '%s' does not contain a video stream!", address.c_str());
        throw std::runtime_error("No video stream at address.");
    }

    /* Allocate video CODEC Context. */
    ptr_codec_context = avcodec_alloc_context3(ptr_codec);
    if (!ptr_codec_context) {
        LOGE("Failed to allocated memory for AVCodecContext.");
        throw std::runtime_error("Failed to allocated memory for AVCodecContext");
    }

    /* Fill the codec context based on the values from the supplied codec parameters */
    if (!ptr_codec_parameters) {
        LOGW("No CODEC Parameters");
    }
    
    if (avcodec_parameters_to_context(ptr_codec_context, ptr_codec_parameters) < 0) {
        LOGE("Failed to copy codec params to codec context.");
        throw std::runtime_error("Failed to copy CODEC params to codec context");
    }

    /* Initialize the AVCodecContext to use the given AVCodec. */
    if (avcodec_open2(ptr_codec_context, ptr_codec, NULL) < 0) {
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

    /* Create Userspace Frame Buffer. */
    frame_data_ = {};
    frame_data_.image = Image(ptr_frame->width, ptr_frame->height, PixelFormat::YUV422P);

    /* Allocate Packet */
    ptr_packet = av_packet_alloc();
    if (!ptr_packet) {
        LOGE("Failed to allocate memory for AVPacket.");
        throw std::runtime_error("Failed to allocate memory for AVPacket");
    }
}

VideoReciever::~VideoReciever() {
    av_packet_free(&ptr_packet);
    av_frame_free(&ptr_frame);
    avformat_close_input(&ptr_format_context);
    avformat_free_context(ptr_format_context);
    avcodec_free_context(&ptr_codec_context);
    av_dict_free(&ptr_open_container_opts);
}

void VideoReciever::iteration() {
    // TODO: Verify FPS not > 60/30FPS.
    recieve(); // Blocking
}

void VideoReciever::setup() {
    LOGI("Running VideoReciever (TID = %d)", gettid());
};

/**
 * @brief Recieve a frame over the network.
 */
void VideoReciever::recieve() {
    int response = 0;
    bool found_packet = false;
    bool decoded_frame = false;

    /* Try to decode frame. */
    response = avcodec_receive_frame(ptr_codec_context, ptr_frame);
    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
        decoded_frame = false;
    } else if (response < 0) {
        LOGW("Issue while receiving a frame from the decoder: %d", (response));
        return;
    } else {
        decoded_frame = true;
    }

    /* Retrieve Video Stream Packets until we can decode the next frame (BLOCKING). */
    while (!decoded_frame && av_read_frame(ptr_format_context, ptr_packet) >= 0) {

        /* Only process selected Video Stream Packets. */
        if (ptr_packet->stream_index == video_stream_index) {
            // LOGI("AVPacket->pts %" PRId64 "\t", ptr_packet->pts);

            /* Send packet to decoder */
            response = avcodec_send_packet(ptr_codec_context, ptr_packet);
            if (response < 0) {
                LOGW("Issue while sending a packet to the decoder: %d", (response));
                return;
            }

            /* Decode new frame */
            response = avcodec_receive_frame(ptr_codec_context, ptr_frame);
            if (response != AVERROR(EAGAIN) && response != AVERROR_EOF && response < 0) {
                LOGW("Issue while receiving a frame from the decoder: %d", (response));
                return;
            } else if (response >= 0) {
                decoded_frame = true;
            }
        }

        av_packet_unref(ptr_packet);
    }

    if (!decoded_frame) {
        return; 
    }

    // double play_time_ = static_cast<double>(ptr_frame->pts) * static_cast<double>(ptr_format_context->streams[video_stream_index]->time_base.num) / static_cast<double>(ptr_format_context->streams[video_stream_index]->time_base.den);
    // LOGI(
    //     "Frame %d (type=%c, size=%d bytes, format=%d) pts %ld key_frame %d time %fs [DTS %d]",
    //     ptr_codec_context->frame_number,
    //     av_get_picture_type_char(ptr_frame->pict_type),
    //     ptr_frame->pkt_size,
    //     ptr_frame->format,
    //     ptr_frame->pts,
    //     ptr_frame->key_frame,
    //     play_time_,
    //     ptr_frame->coded_picture_number
    // );

    /* Process New Frame. */
    if (ptr_frame->format != AV_PIX_FMT_YUV422P) {
        LOGW("Currnelty only YUV422P is supported.");
    }

    ImageView image_view = ImageView(
        {ptr_frame->data[0], ptr_frame->data[1], ptr_frame->data[2]},
        {ptr_frame->linesize[0], ptr_frame->linesize[1], ptr_frame->linesize[2]},
        frame_data_.image.getWidth(), frame_data_.image.getHeight(), PixelFormat::YUV422P
    );
    std::lock_guard lock(frame_data_mutex_);
    ImageView buffer_view = frame_data_.image.view();
    buffer_view.copyFrom(image_view);

    return;
}

/**
 * @brief Get the last frame.
 */
Frame VideoReciever::getFrame(double curr_time, PixelFormat fmt) {
    std::lock_guard lock(frame_data_mutex_);
    frame_data_.image.to(fmt);
    return frame_data_;
}