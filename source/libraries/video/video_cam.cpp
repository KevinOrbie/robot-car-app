/* ============================ Includes ============================ */
#include "video_cam.h"

/* C Standard Libs. */
#include <poll.h>    // poll()
#include <fcntl.h>   // Manipulate file descriptor: open()
#include <errno.h>   // errno
#include <unistd.h>  // close()
#include <assert.h>  // assert()
#include <string.h>  // strerror(), memcpy()

/* Standard C++ Libraries */
#include <stdexcept>

/* OS provided C extentions. */
#include <sys/stat.h>     // Data returned by the stat() function 
#include <sys/mman.h>     // Memory management declarations
#include <sys/ioctl.h>    // Control IO device

/* Thirdparty includes. */
#include <linux/videodev2.h>

/* Custom C++ Libraries */
#include "common/logger.h"
#include "video/image.h"


/* ============================ Defines ============================ */
#define CLEAR(x) memset(&(x), 0, sizeof(x))


/* =========================== Functions =========================== */
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

VideoCam::VideoCam(CamType type, IO_Method io_method, std::string device_name): cam_type_(type), io_method_(io_method), device_name_(device_name) {
    /* ------------ Open Camera Device ------------ */
    struct stat st;

    if (stat(device_name_.c_str(), &st) == -1) {
        LOGE("Cannot identify '%s' (error %d: %s)", device_name_.c_str(), errno, strerror(errno));
        throw std::runtime_error("Failed to read device file");
    }

    if (!S_ISCHR(st.st_mode)) {
        LOGE("%s is not a device!", device_name_.c_str());
        throw std::runtime_error("File is not a device");
    }

    // Mode: O_RDWR = read/write
    // Mode: O_NONBLOCK = None of the I/O operations on the fd will cause the calling process to wait.
    fd_ = open(device_name_.c_str(), O_RDWR | O_NONBLOCK, 0);

    if (fd_ == -1) {
        LOGE("Cannot open '%s' (error %d: %s)", device_name_.c_str(), errno, strerror(errno));
        throw std::runtime_error("Cannot open device file");
    }

    LOGI("Opened Camera Device.");

    /* ------------ Setup & Verify Capabilities ------------ */
    struct v4l2_capability cap;

    /* Query device capabilities. */
    if (xioctl(fd_, VIDIOC_QUERYCAP, &cap) == -1) {
        if (errno == EINVAL) {
            LOGE("%s is not a V4L2 device.", device_name_.c_str());
            throw std::runtime_error("Not a V4L2 device file.");
        } else {
            LOGE("VIDIOC_QUERYCAP issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("VIDIOC_QUERYCAP failed");
        }
    }

    /* Check if the device supports the single-planar API. */
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        LOGE("%s is not a video capture device.", device_name_.c_str());
        throw std::runtime_error("Not a video capture device");
    }

    switch (io_method) {
        case IO_Method::READ:
            /* Does the device support the read() and/or write() I/O methods? */
            if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                LOGE("%s does not support read i/o!", device_name_.c_str());
                throw std::runtime_error("Device does not support read i/o");
            }
            break;

        case IO_Method::MMAP:
        case IO_Method::USERPTR:
            /* Does the device support the streaming I/O method? */
            if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                LOGE("%s does not support read i/o!", device_name_.c_str());
                throw std::runtime_error("Device does not support read i/o");
            }
            break;
    }

    LOGI("Setup & Verified Capabilities.");

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

    LOGI("Setup & Verified Cropping / Scaling.");

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
            LOGE("Unsupported Camera Type used (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("Unsupported Camera Type used");
            break;
    }

    /* Set format info. */
    if (xioctl(fd_, VIDIOC_S_FMT, &fmt) == -1) {
        LOGE("VIDIOC_S_FMT issue (error %d: %s)", errno, strerror(errno));
        throw std::runtime_error("VIDIOC_S_FMT failed");
    }

    frame_bytes_per_line_ = fmt.fmt.pix.bytesperline;  /* Distance in bytes between the leftmost pixels in two adjacent lines. */

    /* Create Userspace Frame Buffer. */
    frame_data_ = {};
    frame_data_.image = Image(fmt.fmt.pix.width, fmt.fmt.pix.height, PixelFormat::YUV422);
    frame_data_.image.zero();

    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;

    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;

    LOGI("Setup & Verified Video Format.");
    
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

    LOGI("Initialized IO Memory.");
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
            LOGE("VIDIOC_QUERYCTRL issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("VIDIOC_QUERYCTRL failed");
        } else {
            LOGW("Camera control is not supported: id=%d.", control_id);
        }
    } else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
        LOGW("Camera control is not supported: id=%d.", control_id);
    } else {
        /* Update Control. */
        CLEAR(control);
        control.id = control_id;
        control.value = value; 

        /* Set a specifc control value. */
        if (xioctl(fd_, VIDIOC_S_CTRL, &control) == -1) {
            LOGE("VIDIOC_S_CTRL issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("VIDIOC_S_CTRL failed");
        }
    }
}

void VideoCam::init_IO_READ(unsigned int size){
    // Uses only one buffer of 1 imagesize (via malloc)
    buffers_.push_back(buffer());

    if (buffers_.empty()) {
        LOGE("Out of memory!");
        throw std::runtime_error("Out of memory");
    }

    buffers_[0].length = size;
    buffers_[0].start = (uint8_t*) malloc(size);

    if (!buffers_[0].start) {
        LOGE("Out of memory!");
        throw std::runtime_error("Out of memory");
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
            LOGE("%s does not support memory mapping.", device_name_.c_str());
            throw std::runtime_error("Memory Mapping not supported");
        } else {
            LOGE("VIDIOC_REQBUFS issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("VIDIOC_REQBUFS failed");
        }
    }

    /* Verify granted number of Buffers */
    if (req.count < 2) {
        LOGE("Insufficient buffer memory on %s.", device_name_.c_str());
        throw std::runtime_error("Insufficient buffer memory");
    }

    // buffers_ = (VideoCam::buffer*) calloc(req.count, sizeof(*buffers_));

    /* Initialize count buffers. */
    buffers_.resize(req.count);
    if (buffers_.empty()) {
        LOGE("Out of memory!");
        throw std::runtime_error("Out of memory");
    }

    for (int buff_idx = 0; buff_idx < req.count; ++buff_idx) {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = buff_idx;

        /* Query the status of a buffer. */
        if (xioctl(fd_, VIDIOC_QUERYBUF, &buf) == -1) {
            LOGE("VIDIOC_QUERYBUF issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("VIDIOC_QUERYBUF failed");
        }

        buffers_[buff_idx].length = buf.length;
        buffers_[buff_idx].start = (uint8_t*)
            mmap(NULL                   /* addr: the kernel chooses the (page-aligned) address at which to create the mapping. */,
                buf.length,             /* length: specifies the length of the mapping. */
                PROT_READ | PROT_WRITE  /* prot: allows for READ & WRITE access (required). */,
                MAP_SHARED              /* flags: updates to the mapping are visible to other processes mapping the same region (recommended). */,
                fd_,                    /* fd: file mapping */
                buf.m.offset            /* offset: file content initialization starts from offset of the file beginning. */
            );

        if (buffers_[buff_idx].start == MAP_FAILED) {
            LOGE("mmap issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("mmap failed");
        }
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
            LOGE("%s does not support user pointer i/o!", device_name_.c_str());
            throw std::runtime_error("user pointer i/o not supported");
        } else {
            LOGE("VIDIOC_REQBUFS issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("VIDIOC_REQBUFS failed");
        }
    }

    /* Initialize count buffers. */
    buffers_.resize(req.count);
    if (buffers_.empty()) {
        LOGE("Out of memory!");
        throw std::runtime_error("Out of memory");
    }

    for (int buff_idx = 0; buff_idx < req.count; ++buff_idx) {
        buffers_[buff_idx].length = size;
        buffers_[buff_idx].start = (uint8_t*) malloc(size);

        if (!buffers_[buff_idx].start) {
            LOGE("Out of memory!");
            throw std::runtime_error("Out of memory");
        }
    }
}

/* ############################## START ############################## */

void VideoCam::startStream() {
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

    for (i = 0; i < buffers_.size(); ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        /* Exchange a buffer with the driver */
        if (xioctl(fd_, VIDIOC_QBUF, &buf) == -1) {
            LOGE("VIDIOC_QBUF issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("VIDIOC_QBUF failed");
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* Start streaming I/O */
    if (xioctl(fd_, VIDIOC_STREAMON, &type) == -1) {
        LOGE("VIDIOC_STREAMON issue (error %d: %s)", errno, strerror(errno));
        throw std::runtime_error("VIDIOC_STREAMON failed");
    }

    capturing = true;

    LOGI("Started Capturing Frames.");
    return;
}

void VideoCam::start_IO_USRP() {
    enum v4l2_buf_type type;
    unsigned int i;

    for (i = 0; i < buffers_.size(); ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;
        buf.index = i;
        buf.m.userptr = (unsigned long)buffers_[i].start;
        buf.length = buffers_[i].length;

        /* Exchange a buffer with the driver. */
        if (xioctl(fd_, VIDIOC_QBUF, &buf) == -1) {
            LOGE("VIDIOC_QBUF issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("VIDIOC_QBUF failed");
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* Start streaming I/O. */
    if (xioctl(fd_, VIDIOC_STREAMON, &type) == -1) {
        LOGE("VIDIOC_STREAMON issue (error %d: %s)", errno, strerror(errno));
        throw std::runtime_error("VIDIOC_STREAMON failed");
    }

    LOGI("Started Capturing Frames.");
}

/* ########################### Destructor ########################## */

VideoCam::~VideoCam(){
    /* Stop Capturing Frames */
    if (capturing) {
        stopStream();
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

    /* Close Camera Device */
    if (close(fd_) == -1) {
        LOGE("close issue (error %d: %s)", errno, strerror(errno));
    }

    fd_ = -1;
    LOGI("Destructed VideoCam.");
}

/* ############################## STOP ############################# */

void VideoCam::stopStream() {
    switch (io_method_) {
        case IO_Method::READ:
            stop_IO_READ();
            break;

        case IO_Method::MMAP:
        case IO_Method::USERPTR:
            stop_IO_STREAM();
            break;
    }
    
    LOGI("Stopped Capturing Frames.");
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
    if (xioctl(fd_, VIDIOC_STREAMOFF, &type) == -1) {
        LOGE("VIDIOC_STREAMOFF issue (error %d: %s)", errno, strerror(errno));
        throw std::runtime_error("VIDIOC_STREAMOFF failed");
    }

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
    for (i = 0; i < buffers_.size(); ++i)
        if (munmap(buffers_[i].start, buffers_[i].length) == -1) {
            LOGE("munmap issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("munmap failed");
        }
    return;
}

void VideoCam::uinit_IO_USRP() {
    unsigned int i;
    for (i = 0; i < buffers_.size(); ++i)
        free(buffers_[i].start);
    return;
}

/* ############################## GetFrame ############################ */

Frame VideoCam::getFrame(double curr_time, PixelFormat fmt){
    frame_data_.image.to(fmt);

    int poll_result = -1;
    int poll_timeout_ms = 20000; // Max wait 20 seconds
    struct pollfd poll_fds;

    /* Setup Poll FD Settings */
    poll_fds.fd = fd_;
    poll_fds.events = POLLIN;  // Wait for available data to read

    while (true) {
        /* Block for I/O operations. */
        poll_result = poll(&poll_fds, 1, poll_timeout_ms);

        /* Poll() returned errors. */
        if (poll_result == -1) {
            if (errno == EINTR)
                continue;
            LOGE("Poll issue (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("poll failed");
        }

        /* Poll() timed outed. */
        if (poll_result == 0) {
            LOGE("Poll timeout.");
            throw std::runtime_error("Poll timeout");  // Currently Limits GUI to camera framerate
        }

        /* Check for ERROR. */
        if (poll_fds.revents & POLLERR) { 
            LOGE("Poll issue (error %d: %s). Make sure camera is started / running.", errno, strerror(errno));
            throw std::runtime_error("poll failed");
        }

        /* Check for closed stream. */
        if (poll_fds.revents & POLLHUP) { 
            LOGE("Device stream has been closed!");
            throw std::runtime_error("Device stream has been closed");
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
        break; // Uncomment if you need a new frame for every call.
    }

    LOGE("Broke out of getFrame loop."); // Never reached.
    return frame_data_;
}

void VideoCam::readFrame(unsigned int buffer_index) {
    switch (cam_type_) {
        case CamType::MYNT_EYE_SINGLE:
        case CamType::MYNT_EYE_STEREO:
        case CamType::ARKMICRO_WEBCAM: {
            /* Directly copy YUV422 to YUV422. */
            ImageView image_view = ImageView(
                {buffers_[buffer_index].start}, {frame_bytes_per_line_},
                frame_data_.image.getWidth(), frame_data_.image.getHeight(), PixelFormat::YUV422
            );

            ImageView buffer_view = frame_data_.image.view();
            buffer_view.copyFrom(image_view);
            break;
        }
        
        default: {
            LOGE("Unsupported Camera Type used (error %d: %s)", errno, strerror(errno));
            throw std::runtime_error("Unsupported Camera Type used");
        }
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
                LOGE("read issue (error %d: %s)", errno, strerror(errno));
                throw std::runtime_error("read failed");
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
                LOGE("VIDIOC_DQBUF issue (error %d: %s)", errno, strerror(errno));
                throw std::runtime_error("VIDIOC_DQBUF failed");
        }
    }

    assert(buf.index < buffers_.size());

    readFrame(buf.index);

    /* Enqueue an empty buffer in the driver’s incoming queue. */
    if (xioctl(fd_, VIDIOC_QBUF, &buf) == -1) {
        LOGE("VIDIOC_QBUF issue (error %d: %s)", errno, strerror(errno));
        throw std::runtime_error("VIDIOC_QBUF failed");
    }

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
                LOGE("VIDIOC_DQBUF issue (error %d: %s)", errno, strerror(errno));
                throw std::runtime_error("VIDIOC_DQBUF failed");
        }
    }

    for (i = 0; i < buffers_.size(); ++i)
        if (buf.m.userptr == (unsigned long)buffers_[i].start
            && buf.length == buffers_[i].length)
                break;

    assert(i < buffers_.size());

    readFrame(buf.index);

    /* Enqueue an empty buffer in the driver’s incoming queue. */
    if (xioctl(fd_, VIDIOC_QBUF, &buf) == -1) {
        LOGE("VIDIOC_QBUF issue (error %d: %s)", errno, strerror(errno));
        throw std::runtime_error("VIDIOC_QBUF failed");
    }

    return true;
}