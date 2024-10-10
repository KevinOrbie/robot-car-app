/**
 * @file video_file.cpp
 * @author Kevin Orbie
 * 
 * @brief C++ wrapper around ffmpeg functionality.
 * @link based on: https://github.com/leandromoreira/ffmpeg-libav-tutorial?tab=readme-ov-file#learn-ffmpeg-libav-the-hard-way
 */

/* ============================ Includes ============================ */
#include "video_file.h"

/* Standard C++ Libraries */
#include <stdexcept>

/* Custom C++ Libraries */
#include "common/logger.h"
#include "image.h"


/* ============================ Classes ============================ */
VideoFile::VideoFile(std::string const& filepath) {
    filepath_ = filepath;

    /* ---------------- Setup Container Context ----------------- */
    ptr_format_context = avformat_alloc_context();
    if (!ptr_format_context) {
        LOGE("Could not allocate memory for Format Context.");
        throw std::runtime_error("Failed to allocate memory for Format Context.");
    }
    
    /* Get Header Information */
    if (avformat_open_input(&ptr_format_context, filepath.c_str(), NULL, NULL) != 0) {
        LOGE("Could not open the video file.");
        throw std::runtime_error("Failed to open video file.");
    }

    LOGI("format %s, duration %ld us, bit_rate %ld", ptr_format_context->iformat->name, ptr_format_context->duration, ptr_format_context->bit_rate);

    /* Get Stream Information */
    if (avformat_find_stream_info(ptr_format_context,  NULL) < 0) {
        LOGE("Could not get the stream info.");
        throw std::runtime_error("Could not get the stream info.");
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

        if (ptr_local_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                /* Use first video CODEC as main codec. */
                video_stream_index = i;
                ptr_codec = ptr_local_codec;
                ptr_codec_parameters = ptr_local_codec_params;
            }
            LOGI("Video Codec: resolution %d x %d", ptr_local_codec_params->width, ptr_local_codec_params->height);
        } 
        
        /* Ignore the audio streams. */
        // if (ptr_local_codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
        //     LOGI("Audio Codec: %d channels, sample rate %d", ptr_local_codec_params->channels, ptr_local_codec_params->sample_rate);
        // }

        LOGI("\tCodec %s ID %d bit_rate %ld", ptr_local_codec->name, ptr_local_codec->id, ptr_local_codec_params->bit_rate);
    }

    /* Verify a video stream was found. */
    if (video_stream_index == -1) {
        LOGE("File %s does not contain a video stream!", filepath.c_str());
        throw std::runtime_error("No video stream in file");
    }

    /* Allocate video CODEC Context. */
    ptr_codec_context = avcodec_alloc_context3(ptr_codec);
    if (!ptr_codec_context)
    {
        LOGE("Failed to allocated memory for AVCodecContext.");
        throw std::runtime_error("Failed to allocated memory for AVCodecContext");
    }

    /* Fill the codec context based on the values from the supplied codec parameters */
    if (avcodec_parameters_to_context(ptr_codec_context, ptr_codec_parameters) < 0)
    {
        LOGE("Failed to copy codec params to codec context.");
        throw std::runtime_error("Failed to copy CODEC params to codec context");
    }

    /* Initialize the AVCodecContext to use the given AVCodec. */
    if (avcodec_open2(ptr_codec_context, ptr_codec, NULL) < 0)
    {
        LOGE("Failed to open CODEC through avcodec_open2.");
        throw std::runtime_error("Failed to open CODEC through avcodec_open2");
    }

    /* ---------------- Allocate Packet / Frame ----------------- */
    /* Allocate Frame. */
    ptr_frame = av_frame_alloc();
    if (!ptr_frame) {
        LOGE("Failed to allocate memory for AVFrame.");
        throw std::runtime_error("Failed to allocate memory for AVFrame.");
    }

    ptr_frame->format = ptr_codec_context->pix_fmt;
    ptr_frame->width  = ptr_codec_context->width;
    ptr_frame->height = ptr_codec_context->height;

    /* Allocate frame data (data buffer). */
    if (av_frame_get_buffer(ptr_frame, 0) < 0) {
        LOGE("Failed to allocate memory for AVFrame data.");
        throw std::runtime_error("Failed to allocate memory for AVFrame data");
    }
    av_frame_make_writable(ptr_frame);

    /* Create Userspace Frame Buffer. */
    frame_data_ = {};
    frame_data_.image = Image(ptr_frame->width, ptr_frame->height, PixelFormat::YUV420P);

    /* Allocate Packet. */
    ptr_packet = av_packet_alloc();
    if (!ptr_packet) {
        LOGE("Failed to allocate memory for AVPacket.");
        throw std::runtime_error("Failed to allocate memory for AVPacket");
    }
}

VideoFile:: ~VideoFile() {
    avformat_close_input(&ptr_format_context);
    av_packet_free(&ptr_packet);
    av_frame_free(&ptr_frame);
    avcodec_free_context(&ptr_codec_context);
}

Frame VideoFile::getFrame(double curr_time, PixelFormat fmt) {
    frame_data_.image.to(fmt);

    int response = 0;
    bool found_packet = false;
    bool decoded_frame = false;
    
    /* Set correct FPS. */
    if (play_time_ >= curr_time) {
        return frame_data_;
    }

    /* Try to decode frame. */
    response = avcodec_receive_frame(ptr_codec_context, ptr_frame);
    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
        decoded_frame = false;
    } else if (response < 0) {
        LOGW("Issue while receiving a frame from the decoder: %d", (response));
        return frame_data_;
    } else {
        decoded_frame = true;
    }

    /* Retrieve Video Stream Packets until we can decode the next frame. */
    while (!decoded_frame && av_read_frame(ptr_format_context, ptr_packet) >= 0) {

        /* Only process selected Video Stream Packets. */
        if (ptr_packet->stream_index == video_stream_index) {
            // LOGI("AVPacket->pts %" PRId64 "\t", ptr_packet->pts);

            /* Send packet to decoder */
            response = avcodec_send_packet(ptr_codec_context, ptr_packet);
            if (response < 0) {
                LOGW("Issue while sending a packet to the decoder: %d", (response));
                return frame_data_;
            }

            /* Decode new frame */
            response = avcodec_receive_frame(ptr_codec_context, ptr_frame);
            if (response != AVERROR(EAGAIN) && response != AVERROR_EOF && response < 0) {
                LOGW("Issue while receiving a frame from the decoder: %d", (response));
                return frame_data_;
            } else if (response >= 0) {
                decoded_frame = true;
            }
        }

        av_packet_unref(ptr_packet);
    }

    if (!decoded_frame) {
        return frame_data_; 
    }

    play_time_ = static_cast<double>(ptr_frame->pts) * static_cast<double>(ptr_format_context->streams[video_stream_index]->time_base.num) / static_cast<double>(ptr_format_context->streams[video_stream_index]->time_base.den);
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

    /* Process Frame. */
    if (ptr_frame->format != AV_PIX_FMT_YUV420P) {
        LOGW("The returned frame may not be a grayscale image, but could e.g. be just the R component if the video format is RGB");
    }

    ImageView image_view = ImageView(
        {ptr_frame->data[0], ptr_frame->data[1], ptr_frame->data[2]},
        {ptr_frame->linesize[0], ptr_frame->linesize[1], ptr_frame->linesize[2]},
        ptr_frame->width, ptr_frame->height, PixelFormat::YUV420P
    );

    ImageView buffer_view = frame_data_.image.view();
    buffer_view.copyFrom(image_view);
    
    return frame_data_;
}
