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
#include <sys/mman.h>     // Memory management declarations
#include <sys/ioctl.h>    // Control IO device

/* Thirdparty includes. */
#include <linux/videodev2.h>

// TODO: Rewrite with C++ syntax (change select to poll(?), smart_ptrs, etc.).
// TODO: Maybe Don't make the control loop linked to the Camera FPS.

// TODO: Add timing logging for pipeline.
// TODO: Write basic unit tests.

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


/* =============================== Classes =============================== */

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
        
        case CamType::MYNT_EYE_SINGLE:
            fmt.fmt.pix.width       = 1280;
            fmt.fmt.pix.height      = 720;
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;  // YUV 4:2:2
            fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED; 
            break;

        case CamType::MYNT_EYE_STEREO:
            fmt.fmt.pix.width       = 2560;
            fmt.fmt.pix.height      = 720;
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;  // YUV 4:2:2
            fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED; 
            break;
        
        default:
            errno_exit("Unsupported Camera Type Used.");
            break;
    }

    /* Set format info. */
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
    
    /* -------------- Set Camera Controls -------------- */
    /* Enable Manual Exposure Control */
    /* NOTE: V4L2_EXPOSURE_MANUAL is not supported, and V4L2_EXPOSURE_SHUTTER_PRIORITY seems to not affect anything directly. */
    // fprintf(stderr, "Setting V4L2_CID_EXPOSURE_AUTO\n");
    // setCamControl(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_AUTO); 

    // /* Set minimal exposure time of the camera sensor (1 = 0.1 ms, 1000 = 0.1s). */
    // fprintf(stderr, "Setting V4L2_CID_EXPOSURE_ABSOLUTE\n");
    // setCamControl(V4L2_CID_EXPOSURE_ABSOLUTE, 100); /* Permission Denied */

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

    // Initialize Frame
    int size = frame_data_.width * frame_data_.height * frame_data_.channels;
    frame_data_.data = std::vector<uint8_t>(size, 0);
}

void VideoCam::setCamControl(unsigned int control_id, int value) {
    /* https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/extended-controls.html?highlight=exposure */
    /* https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/control.html#example-changing-controls */

    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    CLEAR(queryctrl);
    queryctrl.id = control_id;

    /* Verify that control is supported & enabled. */
    if (xioctl(fd_, VIDIOC_QUERYCTRL, &queryctrl) == -1) {
        if (errno != EINVAL) {
            errno_exit("VIDIOC_QUERYCTRL");
            exit(EXIT_FAILURE);
        } else {
            printf("Camera control is not supported: id=%d\n", control_id);
        }
    } else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
        printf("Camera control is not supported: id=%d\n", control_id);
    } else {
        /* Update Control. */
        CLEAR(control);
        control.id = control_id;
        control.value = value; 

        /* Set a specifc control value. */
        if (xioctl(fd_, VIDIOC_S_CTRL, &control) == -1) {
            errno_exit("VIDIOC_S_CTRL");
            exit(EXIT_FAILURE);
        }
    }
}

void VideoCam::init_IO_READ(unsigned int size){
    // Uses only one buffer of 1 imagesize (via malloc)
    buffers_ = (VideoCam::buffer*) calloc(1, sizeof(*buffers_));

    if (!buffers_) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    buffers_[0].length = size;
    buffers_[0].start = (uint8_t*) malloc(size);

    if (!buffers_[0].start) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
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

    buffers_ = (VideoCam::buffer*) calloc(req.count, sizeof(*buffers_));
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

    struct v4l2_requestbuffers req;
    CLEAR(req);

    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;

    /* Switches the driver into user pointer I/O mode and setup some internal structures. */
    if (xioctl(fd_, VIDIOC_REQBUFS, &req) == -1) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support user pointer i/o \n", device_name_.c_str());
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_REQBUFS");
        }
    }

    buffers_ = (VideoCam::buffer*) calloc(4, sizeof(*buffers_));
    if (!buffers_) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
        buffers_[n_buffers].length = size;
        buffers_[n_buffers].start = (uint8_t*) malloc(size);

        if (!buffers_[n_buffers].start) {
            fprintf(stderr, "Out of memory\n");
            exit(EXIT_FAILURE);
        }
    }
}

/* ############################## START ############################## */

void VideoCam::start() {
    switch (io_method_) {
        case IO_Method::READ:
            start_IO_READ();
            break;

        case IO_Method::MMAP:
            start_IO_MMAP();
            break;

        case IO_Method::USERPTR:
            start_IO_USRP();
            break;
    }

    return;
}

void VideoCam::start_IO_READ() {
    /* Nothing to do. */
    return;
}

void VideoCam::start_IO_MMAP() {
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
    return;
}

void VideoCam::start_IO_USRP() {
    enum v4l2_buf_type type;
    unsigned int i;

    for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;
        buf.index = i;
        buf.m.userptr = (unsigned long)buffers_[i].start;
        buf.length = buffers_[i].length;

        /* Exchange a buffer with the driver. */
        if (xioctl(fd_, VIDIOC_QBUF, &buf) == -1)
            errno_exit("VIDIOC_QBUF");
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* Start streaming I/O. */
    if (xioctl(fd_, VIDIOC_STREAMON, &type) == -1)
            errno_exit("VIDIOC_STREAMON");
}

/* ########################### Destructor ########################## */

VideoCam::~VideoCam(){
    /* Stop Capturing Frames */
    if (capturing) {
        stop();
    }

    /* Unitinitalize Device */
    switch (io_method_) {
        case IO_Method::READ:
            uinit_IO_READ();
            break;

        case IO_Method::MMAP:
            uinit_IO_MMAP();
            break;

        case IO_Method::USERPTR:
            uinit_IO_USRP();
            break;
    }

    free(buffers_);

    /* Close Camera Device */
    if (close(fd_) == -1)
        errno_exit("close");

    fd_ = -1;
    fprintf(stderr, "Destructed VideoCam\n");
}

/* ############################## STOP ############################# */

void VideoCam::stop() {
    switch (io_method_) {
        case IO_Method::READ:
            stop_IO_READ();
            break;

        case IO_Method::MMAP:
        case IO_Method::USERPTR:
            stop_IO_STREAM();
            break;
    }
    
    fprintf(stderr, "Stopped Capturing Frames\n");
    return;
}

void VideoCam::stop_IO_READ() {
    /* Nothing to do. */
    return;
}

void VideoCam::stop_IO_STREAM() {
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* Stop streaming I/O. */
    if (xioctl(fd_, VIDIOC_STREAMOFF, &type) == -1)
        errno_exit("VIDIOC_STREAMOFF");

    capturing = false;
    return;
}

/* ############################## UN-Init ############################# */

void VideoCam::uinit_IO_READ() {
    free(buffers_[0].start);
    return;
}

void VideoCam::uinit_IO_MMAP() {
    unsigned int i;
    for (i = 0; i < n_buffers; ++i)
        if (munmap(buffers_[i].start, buffers_[i].length) == -1)
            errno_exit("munmap");
    return;
}

void VideoCam::uinit_IO_USRP() {
    unsigned int i;
    for (i = 0; i < n_buffers; ++i)
        free(buffers_[i].start);
    return;
}

/* ############################## GetFrame ############################ */

Frame* VideoCam::getFrame(double curr_time){
    while (true) {
        fd_set fds;         /* Represent a set of file descriptors. */
        struct timeval tv;  /* Specifies the interval that select() should block waiting for a file descriptor to become ready. */
        int r;

        FD_ZERO(&fds);
        FD_SET(fd_, &fds);

        /* Reset Timeout. */
        tv.tv_sec = 5;   /* Seconds */
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
                    return &frame_data_;
                }
                break;

            case IO_Method::MMAP:
                if (getFrame_IO_MMAP()) {
                    return &frame_data_;
                }
                break;

            case IO_Method::USERPTR:
                if (getFrame_IO_USRP()) {
                    return &frame_data_;
                }
                break;
            
            default:
                break;
        }

        /* NOTE: Working with Non-Blocking I/O, this point is only reached after recieving a EAGAIN error,
        i.e. there is no data available right now, try again later. Thus, we try again to read a frame. */
        break; // Uncomment if you need a new frame for every call.
    }

    fprintf(stderr, "Broke out of get frame loop.\n"); // Never reached.
    return &frame_data_;
}

void VideoCam::readFrame(unsigned int buffer_index){
    int size = frame_data_.width * frame_data_.height * frame_data_.channels;
    frame_data_.data.resize(size);

    switch (cam_type_) {
        case CamType::MYNT_EYE_STEREO:
        case CamType::ARKMICRO_WEBCAM:
            for (int yidx = 0; yidx < frame_data_.height; yidx++) {
                for (int xidx = 0; xidx < frame_data_.width; xidx += 2) {
                    int idx = (yidx * frame_data_.width + xidx) * frame_data_.channels;

                    // NOTE: Here, YUV values are interleaved, not in planar order.
                    // NOTE: frame_bytes_per_line_ is the width of the image in memory (>= width)
                    // NOTE: YUV 422 has 1 Cr & 1 Cb value per 2 Y values (YUYV = 2 pixels, using same U,V)

                    /* Read even pixel. */
                    frame_data_.data[idx + 0] = *(buffers_[buffer_index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 0);  // Y1
                    frame_data_.data[idx + 1] = *(buffers_[buffer_index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 1);  // U (use for even / uneven pixel)
                    frame_data_.data[idx + 2] = *(buffers_[buffer_index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 3);  // V (use for even / uneven pixel)
                    idx += 3;

                    /* Read uneven pixel. */
                    frame_data_.data[idx + 0] = *(buffers_[buffer_index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 2);  // Y2
                    frame_data_.data[idx + 1] = *(buffers_[buffer_index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 1);  // U (use for even / uneven pixel)
                    frame_data_.data[idx + 2] = *(buffers_[buffer_index].start + yidx * frame_bytes_per_line_ + xidx * 2 + 3);  // V (use for even / uneven pixel)
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
    if (read(fd_, buffers_[0].start, buffers_[0].length) == -1) {
        switch (errno) {
            case EAGAIN:
                return false;

            case EIO:
                /* Could ignore EIO, see spec. */
                /* fall through */

            default:
                errno_exit("read");
        }
    }

    readFrame(0);

    return true;
}

bool VideoCam::getFrame_IO_MMAP() {
    struct v4l2_buffer buf;
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

    readFrame(buf.index);

    /* Enqueue an empty buffer in the driver’s incoming queue. */
    if (xioctl(fd_, VIDIOC_QBUF, &buf) == -1)
        errno_exit("VIDIOC_QBUF");

    return true;
}

bool VideoCam::getFrame_IO_USRP() {
    struct v4l2_buffer buf;
    unsigned int i;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;

    /* Dequeue a filled buffer from the driver’s outgoing queue. */
    if (xioctl(fd_, VIDIOC_DQBUF, &buf) == -1) {
        switch (errno) {
            case EAGAIN:
                return true;

            case EIO:
                /* Could ignore EIO, see spec. */
                /* fall through */

            default:
                errno_exit("VIDIOC_DQBUF");
        }
    }

    for (i = 0; i < n_buffers; ++i)
        if (buf.m.userptr == (unsigned long)buffers_[i].start
            && buf.length == buffers_[i].length)
                break;

    assert(i < n_buffers);

    readFrame(buf.index);

    /* Enqueue an empty buffer in the driver’s incoming queue. */
    if (xioctl(fd_, VIDIOC_QBUF, &buf) == -1)
            errno_exit("VIDIOC_QBUF");

    return true;
}