/**
 * @link https://github.com/leandromoreira/ffmpeg-libav-tutorial?tab=readme-ov-file#learn-ffmpeg-libav-the-hard-way
 */

/* ============================ Includes ============================ */
#include "video_file.h"


/* ============================ Classes ============================ */
VideoFile::VideoFile(std::string const& filepath) {
    filepath_ = filepath;

    /* ---------------- Setup Container Context ----------------- */
    ptr_format_context = avformat_alloc_context();
    if (!ptr_format_context) {
        printf("ERROR could not allocate memory for Format Context\n");
        return;
    }
    
    /* Get Header Information */
    if (avformat_open_input(&ptr_format_context, filepath.c_str(), NULL, NULL) != 0) {
        printf("ERROR could not open the file\n");
        return;
    }

    printf("format %s, duration %ld us, bit_rate %ld\n", ptr_format_context->iformat->name, ptr_format_context->duration, ptr_format_context->bit_rate);

    /* Get Stream Information */
    if (avformat_find_stream_info(ptr_format_context,  NULL) < 0) {
        printf("ERROR could not get the stream info\n");
        return;
    }

    /* ---------------------- Setup CODEC ----------------------- */
    /* Loop over all streams */
    for (int i = 0; i < ptr_format_context->nb_streams; i++) {
        /* Find stream's CODEC parameters. */
        AVCodecParameters *ptr_local_codec_params =  NULL;
        ptr_local_codec_params = ptr_format_context->streams[i]->codecpar;

        printf("\n ======== Stream %d ========\n", i);
        printf("AVStream->time_base before open coded %d/%d\n", ptr_format_context->streams[i]->time_base.num, ptr_format_context->streams[i]->time_base.den);
        printf("AVStream->r_frame_rate before open coded %d/%d\n", ptr_format_context->streams[i]->r_frame_rate.num, ptr_format_context->streams[i]->r_frame_rate.den);
        printf("AVStream->start_time %ld\n" PRId64, ptr_format_context->streams[i]->start_time);
        printf("AVStream->duration %ld\n" PRId64, ptr_format_context->streams[i]->duration);

        /* Find CODEC decoder for the current stream. */
        AVCodec const *ptr_local_codec = NULL;
        ptr_local_codec = avcodec_find_decoder(ptr_local_codec_params->codec_id);

        if (ptr_local_codec==NULL) {
            printf("ERROR unsupported codec!\n");
            continue;
        }

        if (ptr_local_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                /* Use first video CODEC as main codec. */
                video_stream_index = i;
                ptr_codec = ptr_local_codec;
                ptr_codec_parameters = ptr_local_codec_params;
            }
            printf("Video Codec: resolution %d x %d\n", ptr_local_codec_params->width, ptr_local_codec_params->height);
        } else if (ptr_local_codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
            printf("Audio Codec: %d channels, sample rate %d\n", ptr_local_codec_params->channels, ptr_local_codec_params->sample_rate);
        }

        printf("\tCodec %s ID %d bit_rate %ld\n", ptr_local_codec->name, ptr_local_codec->id, ptr_local_codec_params->bit_rate);
    }
    printf("\n");

    /* Verify a video stream was found. */
    if (video_stream_index == -1) {
        printf("File %s does not contain a video stream!", filepath.c_str());
        return;
    }

    /* Allocate video CODEC Context. */
    ptr_codec_context = avcodec_alloc_context3(ptr_codec);
    if (!ptr_codec_context)
    {
        printf("failed to allocated memory for AVCodecContext");
        return;
    }

    /* Fill the codec context based on the values from the supplied codec parameters */
    if (avcodec_parameters_to_context(ptr_codec_context, ptr_codec_parameters) < 0)
    {
        printf("failed to copy codec params to codec context");
        return;
    }

    /* Initialize the AVCodecContext to use the given AVCodec. */
    if (avcodec_open2(ptr_codec_context, ptr_codec, NULL) < 0)
    {
        printf("failed to open codec through avcodec_open2");
        return;
    }

    /* ---------------- Allocate Packet / Frame ----------------- */
    ptr_frame = av_frame_alloc();
    if (!ptr_frame) {
        printf("failed to allocate memory for AVFrame");
        return;
    }

    ptr_packet = av_packet_alloc();
    if (!ptr_packet) {
        printf("failed to allocate memory for AVPacket");
        return;
    }
}

VideoFile:: ~VideoFile() {
    avformat_close_input(&ptr_format_context);
    av_packet_free(&ptr_packet);
    av_frame_free(&ptr_frame);
    avcodec_free_context(&ptr_codec_context);
}

Frame* VideoFile::getFrame(double curr_time) {
    int response = 0;
    bool found_packet = false;
    bool decoded_frame = false;
    
    /* Set correct FPS. */
    if (play_time_ >= curr_time) {
        return &frame_data_;
    }

    /* Try to decode frame. */
    response = avcodec_receive_frame(ptr_codec_context, ptr_frame);
    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
        decoded_frame = false;
    } else if (response < 0) {
        printf("Error while receiving a frame from the decoder: %d", (response));
        return &frame_data_;
    } else {
        decoded_frame = true;
    }

    /* Retrieve Video Stream Packets until we can decode the next frame. */
    while (!decoded_frame && av_read_frame(ptr_format_context, ptr_packet) >= 0) {

        /* Only process selected Video Stream Packets. */
        if (ptr_packet->stream_index == video_stream_index) {
            printf("AVPacket->pts %" PRId64 "\t", ptr_packet->pts);

            /* Send packet to decoder */
            response = avcodec_send_packet(ptr_codec_context, ptr_packet);
            if (response < 0) {
                printf("Error while sending a packet to the decoder: %d", (response));
                return &frame_data_;
            }

            /* Decode new frame */
            response = avcodec_receive_frame(ptr_codec_context, ptr_frame);
            if (response != AVERROR(EAGAIN) && response != AVERROR_EOF && response < 0) {
                printf("Error while receiving a frame from the decoder: %d", (response));
                return &frame_data_;
            } else if (response >= 0) {
                decoded_frame = true;
            } else {
                printf("\n");
            }
        }

        av_packet_unref(ptr_packet);
    }

    if (!decoded_frame) {
        printf("\n");
        return &frame_data_; 
    }

    play_time_ = static_cast<double>(ptr_frame->pts) * static_cast<double>(ptr_format_context->streams[video_stream_index]->time_base.num) / static_cast<double>(ptr_format_context->streams[video_stream_index]->time_base.den);
    printf(
        "Frame %d (type=%c, size=%d bytes, format=%d) pts %ld key_frame %d time %fs [DTS %d]\n",
        ptr_codec_context->frame_number,
        av_get_picture_type_char(ptr_frame->pict_type),
        ptr_frame->pkt_size,
        ptr_frame->format,
        ptr_frame->pts,
        ptr_frame->key_frame,
        play_time_,
        ptr_frame->coded_picture_number
    );

    /* Process Frame. */
    if (ptr_frame->format != AV_PIX_FMT_YUV420P) {
        printf("Warning: the generated file may not be a grayscale image, but could e.g. be just the R component if the video format is RGB");
    }

    frame_data_.width = ptr_frame->width;
    frame_data_.height = ptr_frame->height;
    frame_data_.channels = 3;

    int size = frame_data_.width * frame_data_.height * frame_data_.channels;
    frame_data_.data.resize(size);

    for (int yidx = 0; yidx < frame_data_.height; yidx++) {
        for (int xidx = 0; xidx < frame_data_.width; xidx++) {
            int idx = (yidx * frame_data_.width + xidx) * frame_data_.channels;

            // NOTE: linesize is the width of the image in memory (>= width)
            // NOTE: YUV 420 has 1 Cr & 1 Cb value per 2x2 Y-block

            frame_data_.data[idx + 0] = *(ptr_frame->data[0] + yidx * ptr_frame->linesize[0] + xidx);        // Y
            frame_data_.data[idx + 1] = *(ptr_frame->data[1] + (yidx/2) * ptr_frame->linesize[1] + xidx/2);  // U (use for even / uneven pixel)
            frame_data_.data[idx + 2] = *(ptr_frame->data[2] + (yidx/2) * ptr_frame->linesize[2] + xidx/2);  // V (use for even / uneven pixel)
        }
    }

    return &frame_data_;
}
