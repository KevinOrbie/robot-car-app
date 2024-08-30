/**
 * @brief C++ wrapper around ffmpeg functionality.
 */

/* ========================== Include ========================== */
#include "frame_provider.h"

/* Thirdparty includes. */
#include <linux/videodev2.h>

/* ========================== Classes ========================== */

/**
 * @brief Class to obtain frames from a camera device (on linux).
 * 
 * @example {@code
 *  VideoCam camera = VideoCam();
 *  camera.start();
 *  camera.getFrame(time);
 *  camera.getFrame(time);
 *  camera.stop();
 * }
 */
class VideoCam: public FrameProvider {
    /* --------------- Classes / Structures --------------- */
    struct buffer {
        uint8_t  *start;
        size_t   length;
    };

   public:
    enum class CamType {
        ARKMICRO_WEBCAM
    };

    enum class IO_Method {
        READ,       // Use simple read() calls.
        MMAP,       // Memory mapped buffers are located in device memory.
        USERPTR,    // Application allocated buffers. (User Pointer IO)
    };

    /* --------------- Function Declarations -------------- */
   public:
    VideoCam(CamType type=CamType::ARKMICRO_WEBCAM, IO_Method io_method=IO_Method::MMAP);
    ~VideoCam();
    Frame* getFrame(double curr_time) override;
    void start();
    void stop();

   private:
    void setCamControl(unsigned int control_id, int value);

    void init_IO_READ(unsigned int size);
    void init_IO_MMAP();
    void init_IO_USRP(unsigned int size);

    void uinit_IO_READ();
    void uinit_IO_MMAP();
    void uinit_IO_USRP();

    void start_IO_READ();
    void start_IO_MMAP();
    void start_IO_USRP();

    void stop_IO_READ();
    void stop_IO_STREAM();

    bool getFrame_IO_READ();
    bool getFrame_IO_MMAP();
    bool getFrame_IO_USRP();
    void readFrame(unsigned int buffer_index);

    /* -------------- Variable Declarations -------------- */
   private:
    CamType cam_type_;
    std::string device_name_;
    bool capturing = false;
    
    /* IO variables */
    int fd_ = -1;
    IO_Method io_method_;
    struct buffer *buffers_;
    unsigned int n_buffers = 0;
    int frame_bytes_per_line_ = 0;

    /* Frame Data */
    Frame frame_data_ = {};
};
