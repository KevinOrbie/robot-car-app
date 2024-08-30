/* ============================ Includes ============================ */
#include "video_cam.h"

/* C Standard Libs. */
#include <fcntl.h>   // Manipulate file descriptor: open()
#include <errno.h>   // errno
#include <unistd.h>  // close()
#include <assert.h>  // assert()
#include <string.h>  // strerror(), memcpy()

/* OS provided C extentions. */
#include <sys/stat.h>     // Data returned by the stat() function 
#include <sys/ioctl.h>    // Control IO device
#include <sys/mman.h>     // Memory management declarations

/* Thirdparty includes. */
#include <linux/videodev2.h>

// TODO: Display Frame in OpenGL
// TODO: Test with WebCam

// TODO: Rethink how to store and share Frames.
// TODO: Support other I/O types (Part of constructor call? Different Sub classes?).
// TODO: Rewrite with C++ syntax (change select to poll(?), smart_ptrs, etc.).
// TODO: Remove unneeded functions

// TODO: Test with other camera (double frame?, maybe StereoCam class?)


/* ============================ Defines ============================ */
#define CLEAR(x) memset(&(x), 0, sizeof(x))


/* =========================== Functions =========================== */
static void errno_exit(const char *s) {
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

/**
 * @note: The ioctl(int fd, int op, ...) system call manipulates the 
 * underlying device parameters of special files:
 *  > fd: File Discriptor
 *  > op: Operation Code
 */
static int xioctl(int fh, int request, void *arg) {
    int r;

    do {
        r = ioctl(fh, request, arg);
    } while (r == -1 && errno == EINTR);

    return r;
}


/* ============================ Classes ============================ */
VideoCam::VideoCam(CamType type, IO_Method io_method): cam_type_(type), io_method_(io_method) {
    device_name_ = "/dev/video0";

    /* ------------ Open Camera Device ------------ */
    struct stat st;

    if (stat(device_name_.c_str(), &st) == -1) {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n", device_name_.c_str(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "%s is no device\n", device_name_.c_str());
        exit(EXIT_FAILURE);
    }

    // Mode: O_RDWR = read/write
    // Mode: O_NONBLOCK = None of the I/O operations on the fd will cause the calling process to wait.
    fd_ = open(device_name_.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    if (fd_ == -1) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", device_name_.c_str(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Opened Camera Device\n");

    /* ------------ Setup & Verify Capabilities ------------ */
    struct v4l2_capability cap;

    /* Query device capabilities. */
    if (xioctl(fd_, VIDIOC_QUERYCAP, &cap) == -1) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n", device_name_.c_str());
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_QUERYCAP");
        }
    }

    /* Check if the device supports the single-planar API. */
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n", device_name_.c_str());
        exit(EXIT_FAILURE);
    }

    switch (io_method) {
        case IO_Method::READ:
            /* Does the device support the read() and/or write() I/O methods? */
            if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                fprintf(stderr, "%s does not support read i/o\n", device_name_.c_str());
                exit(EXIT_FAILURE);
            }
            break;

        case IO_Method::MMAP:
        case IO_Method::USERPTR:
            /* Does the device support the streaming I/O method? */
            if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                fprintf(stderr, "%s does not support streaming i/o\n", device_name_.c_str());
                exit(EXIT_FAILURE);
            }
            break;
    }

    fprintf(stderr, "Setup & Verified Capabilities\n");

    /* ------- Setup & Verify Cropping / Scaling ------- */
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;

    CLEAR(cropcap);  // Zero Initialize
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* (UNUSED) Querry Scaling and Cropping Capabilites. */
    if (xioctl(fd_, VIDIOC_CROPCAP, &cropcap) == 0) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (xioctl(fd_, VIDIOC_S_CROP, &crop) == -1) {
            switch (errno) {
                case EINVAL:
                    /* Cropping not supported. */
                    break;
                default:
                    /* Errors ignored. */
                    break;
            }
        }
    } else {
        /* Errors ignored. */
    }

    fprintf(stderr, "Setup & Verified Cropping / Scaling\n");

    /* ---------- Setup & Verify Video Format ---------- */
    struct v4l2_format fmt;
    unsigned int min;

    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    switch (type) {
        case CamType::ARKMICRO_WEBCAM:
            fmt.fmt.pix.width       = 640;
            fmt.fmt.pix.height      = 480;
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;  // YUV 4:2:2
            /* Interlaced video divides an image into two fields, containing only the odd and even lines of the image, respectively.
            It is important to understand a video camera does not expose one frame at a time, merely transmitting the frames separated into fields.
            In Interlaced operation, both top and bottom fields (taken at different times) are transmitted as one image.
            https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/field-order.html#field-order */
            fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED; 
            break;
        
        default:
            errno_exit("Unsupported Camera Type Used.");
            break;
    }

    /* Set format info */
    if (xioctl(fd_, VIDIOC_S_FMT, &fmt) == -1)
        errno_exit("VIDIOC_S_FMT");

    frame_bytes_per_line_ = fmt.fmt.pix.bytesperline;  /* Distance in bytes between the leftmost pixels in two adjacent lines. */
    frame_data_.height = fmt.fmt.pix.height;
    frame_data_.width = fmt.fmt.pix.width;
    frame_data_.channels = 3;

    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;

    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;

    fprintf(stderr, "Setup & Verified Video Format\n");

    /* ------------- Initialize IO Memory -------------- */
    switch (io_method_) {
        case IO_Method::READ:
            /* Use read() calls. */
            init_IO_READ(fmt.fmt.pix.sizeimage);
            break;

        case IO_Method::MMAP: // TRY FIRST
            /* Memory mapped buffers. */
            init_IO_MMAP();
            break;

        case IO_Method::USERPTR:
            /* Application allocated buffers. */
            init_IO_USRP(fmt.fmt.pix.sizeimage);
            break;
    }

    fprintf(stderr, "Initialized IO Memory\n");
}

void VideoCam::init_IO_READ(unsigned int size){
    // Uses one buffer of 1 imagesize (via malloc)
}

void VideoCam::init_IO_MMAP(){
    // Requires V4L2_CAP_STREAMING flag
    // Allocates 4 buffers.
    // Memory mapped buffers are located in device memory and must be allocated with this ioctl before they can be mapped into the application’s address space (using mmap). 
    // Device memory can be for example the video memory on a graphics card with a video capture add-on. 
    // Streaming is an I/O method where only pointers to buffers are exchanged between application and driver, the data itself is not copied
    
    struct v4l2_requestbuffers req;
    CLEAR(req);

    req.count = 4;  // Derised Number of Buffers
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    /* Initiate Memory Mapping */
    if (xioctl(fd_, VIDIOC_REQBUFS, &req) == -1) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support memory mapping\n", device_name_.c_str());
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_REQBUFS");
        }
    }

    /* Verify granted number of Buffers */
    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n", device_name_.c_str());
        exit(EXIT_FAILURE);
    }

    buffers_ = (buffer*) calloc(req.count, sizeof(*buffers_));

    if (!buffers_) {
            fprintf(stderr, "Out of memory\n");
            exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = n_buffers;

        /* Query the status of a buffer */
        if (xioctl(fd_, VIDIOC_QUERYBUF, &buf) == -1)
            errno_exit("VIDIOC_QUERYBUF");

        buffers_[n_buffers].length = buf.length;
        buffers_[n_buffers].start = (uint8_t*)
            mmap(NULL                   /* addr: the kernel chooses the (page-aligned) address at which to create the mapping. */,
                buf.length,             /* length: specifies the length of the mapping. */
                PROT_READ | PROT_WRITE  /* prot: allows for READ & WRITE access (required). */,
                MAP_SHARED              /* flags: updates to the mapping are visible to other processes mapping the same region (recommended). */,
                fd_,                    /* fd: file mapping */
                buf.m.offset            /* offset: file content initialization starts from offset of the file beginning. */
            );

        if (MAP_FAILED == buffers_[n_buffers].start)
            errno_exit("mmap");
    }
}

void VideoCam::init_IO_USRP(unsigned int size){
    // Requires V4L2_CAP_STREAMING flag
    // Makes the application allocate 4 buffers (in user space).
    // Buffers (planes) are allocated by the application itself, and can reside for example in virtual or shared memory. 
    // Switches the driver into user pointer I/O mode and setup some internal structures.
}

void VideoCam::start() {
    // NOTE: only IO_METHOD_MMAP method
    unsigned int i;
    enum v4l2_buf_type type;

    for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        /* Exchange a buffer with the driver */
        if (xioctl(fd_, VIDIOC_QBUF, &buf) == -1)
            errno_exit("VIDIOC_QBUF");
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /* Start streaming I/O */
    if (xioctl(fd_, VIDIOC_STREAMON, &type) == -1)
        errno_exit("VIDIOC_STREAMON");

    capturing = true;

    fprintf(stderr, "Started Capturing Frames\n");
}

void VideoCam::stop() {
    // NOTE: only IO_METHOD_MMAP && IO_METHOD_USERPTR method
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (xioctl(fd_, VIDIOC_STREAMOFF, &type) == -1)
        errno_exit("VIDIOC_STREAMOFF");

    capturing = false;

    fprintf(stderr, "Stopped Capturing Frames\n");
}

VideoCam::~VideoCam(){
    /* ------------ Stop Capturing Frames ------------ */
    if (capturing) {
        stop();
    }

    /* ------------ Unitinitalize Device ------------- */
    unsigned int i;
    for (i = 0; i < n_buffers; ++i)
        if (munmap(buffers_[i].start, buffers_[i].length) == -1)
            errno_exit("munmap");

    /* ------------- Close Camera Device ------------- */
    if (close(fd_) == -1)
        errno_exit("close");

    fd_ = -1;

    fprintf(stderr, "Destructed VideoCam\n");
}

Frame VideoCam::getFrame(double curr_time){
    while (true) {
        fd_set fds;         /* Represent a set of file descriptors. */
        struct timeval tv;  /* Specifies the interval that select() should block waiting for a file descriptor to become ready. */
        int r;

        FD_ZERO(&fds);
        FD_SET(fd_, &fds);

        /* Reset Timeout. */
        tv.tv_sec = 2;   /* Seconds */
        tv.tv_usec = 0;  /* Microseconds */

        /**
         * @brief select() allows a program to monitor multiple file descriptors,
         * waiting until one or more of the file descriptors become "ready"
         * for some class of I/O operation (e.g., input possible).
         *
         * @param nfds:      The highest-numbered file descriptor in any of the given fd_sets, plus 1.
         * @param readfds:   Set of fds, watched to see if they are ready for reading.
         * @param writefds:  (NONE) Set of fds, watched to see if they are ready for writing.
         * @param exceptfds: (NONE) Set of fds, watched for any "exceptional conditions".
         * @param timeout:   Specifies the interval that select() should block
         * 
         * @warning: On Linux, overwrites timeval struct.
         */
        r = select(fd_ + 1, &fds, NULL, NULL, &tv);

        /* Select() returned errors. */
        if (-1 == r) {
            if (EINTR == errno)
                continue;
            errno_exit("select");
        }

        /* Select() timed outed. */
        if (0 == r) {
            fprintf(stderr, "select timeout\n");
            exit(EXIT_FAILURE);
        }

        /* Try to read frame. */
        switch (io_method_) {
            case IO_Method::READ:
                if (getFrame_IO_READ()) {
                    return frame_data_;
                }
                break;

            case IO_Method::MMAP:
                if (getFrame_IO_MMAP()) {
                    return frame_data_;
                }
                break;

            case IO_Method::USERPTR:
                if (getFrame_IO_USRP()) {
                    return frame_data_;
                }
                break;
            
            default:
                break;
        }

        /* NOTE: Working with Non-Blocking I/O, this point is only reached after recieving a EAGAIN error,
        i.e. there is no data available right now, try again later. Thus, we try again to read a frame. */ 
    }
}

void VideoCam::readFrame(v4l2_buffer buf){
    int size = frame_data_.width * frame_data_.height * frame_data_.channels;
    frame_data_.data.resize(size);

    switch (cam_type_) {
        case CamType::ARKMICRO_WEBCAM:
            for (int yidx = 0; yidx < frame_data_.height; yidx++) {
                for (int xidx = 0; xidx < frame_data_.width; xidx += 2) {
                    int idx = (yidx * frame_data_.width + xidx) * frame_data_.channels;

                    // NOTE: Here, YUV values are interleaved, not in planar order.
                    // NOTE: frame_bytes_per_line_ is the width of the image in memory (>= width)
                    // NOTE: YUV 422 has 1 Cr & 1 Cb value per 2 Y values (YUYV = 2 pixels, using same U,V)

                    /* Read even pixel. */
                    frame_data_.data[idx + 0] = *(buffers_[buf.index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 0);  // Y1
                    frame_data_.data[idx + 1] = *(buffers_[buf.index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 1);  // U (use for even / uneven pixel)
                    frame_data_.data[idx + 2] = *(buffers_[buf.index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 3);  // V (use for even / uneven pixel)
                    idx += 3;

                    /* Read uneven pixel. */
                    frame_data_.data[idx + 0] = *(buffers_[buf.index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 2);  // Y2
                    frame_data_.data[idx + 1] = *(buffers_[buf.index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 1);  // U (use for even / uneven pixel)
                    frame_data_.data[idx + 2] = *(buffers_[buf.index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 3);  // V (use for even / uneven pixel)
                }
            }
            break;
        
        default:
            errno_exit("Unsupported Camera Type Used.");
            break;
    }

    return;
}

bool VideoCam::getFrame_IO_READ() {
    return true;
}

bool VideoCam::getFrame_IO_MMAP() {
    struct v4l2_buffer buf;
    Frame err_frame = {};
    unsigned int i;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    /* Dequeue a filled buffer from the driver’s outgoing queue. */
    if (xioctl(fd_, VIDIOC_DQBUF, &buf) == -1) {
        switch (errno) {
            case EAGAIN:
                return false;

            case EIO:
                /* Could ignore EIO, see spec. */
                /* fall through */

            default:
                errno_exit("VIDIOC_DQBUF");
        }
    }

    assert(buf.index < n_buffers);

    readFrame(buf);

    /* Enqueue an empty buffer in the driver’s incoming queue. */
    if (xioctl(fd_, VIDIOC_QBUF, &buf) == -1)
        errno_exit("VIDIOC_QBUF");

    return true;
}

bool VideoCam::getFrame_IO_USRP() {
    return true;
}