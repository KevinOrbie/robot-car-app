/**
 * @brief Main application running on the robot.
 * @author Kevin Orbie
 */

#define ENGINE_VERSION 0

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <unistd.h>  // getopt
#include <sys/stat.h>  // stat

/* Standard C++ Libraries */
#include <iostream>
#include <thread>
#include <memory>

/* Third Party Libraries */
// None

/* Custom C++ Includes */
#include "video/video_transmitter.h"
#include "video/video_file.h"
#include "video/video_cam.h"
#include "robot/arduino_driver.h"
#include "robot/remote.h"


/* ======================== Entry Point ======================== */
static void help() {
    std::string msg = "";

    /* Usage. */
    msg += "usage: engine [options]\n";

    /* Explanation. */
    msg += "\nOptions:\n";
    msg += "  -h              display this help message\n";
    msg += "  -a              enable camera and arduino driver\n";
    msg += "  -m              enable the arduino driver (don't require remote connection)\n";
    msg += "  -d              enable the depth estimation (experimental)\n";
    msg += "  -c              stream from the camera\n";
    msg += "  -v <path>       stream from the video file\n";
    msg += "  -i <address>    ip address of the remote to connect to\n";
    
    msg += "\n";

    fprintf(stderr, "%s", msg.c_str());
};

static void summary(std::string remote_ip, std::string video_file, bool use_camera, bool use_video_file, bool enable_arduino, bool enable_depth) {
    std::string frame_provider = "";
    if (use_camera) {
        frame_provider = "camera";
    } else if (use_video_file) {
        frame_provider = video_file;
    } else {
        frame_provider = "none";
    }

    std::string input_sink = "";
    if (enable_arduino) {
        input_sink = "arduino";
    } else {
        input_sink = "none";
    }

    LOGI("--------- Summary ---------");
    LOGI("  > Frame Provider   : %s", frame_provider.c_str());
    LOGI("  > Depth Estimation : %s", (enable_depth) ? "enabled":"disabled");
    LOGI("  > Input Sink       : %s", input_sink.c_str());
    LOGI("  > Remote IP        : %s", remote_ip.c_str());
    LOGI("---------------------------");
};

int main(int argc, char *argv[]) {
    /* ------------------ Default Values ------------------ */
    std::string remote_ip = "192.168.0.234";
    std::string video_file;

    bool use_camera     = false;
    bool enable_depth   = false;
    bool use_video_file = false;
    bool enable_arduino = false;

    /* ----------------- Parse User Input ----------------- */
    int option;
    while ((option = getopt(argc, argv, "acv:mdi:h")) != -1) {
        switch (option) {
            case 'a': {
                use_camera = true;
                enable_arduino = true;
                break;
            }
            case 'c':
                use_camera = true;
                break;
            case 'm':
                enable_arduino = true;
                break;
            case 'd':
                enable_depth = true;
                break;
            case 'i':
                remote_ip = std::string(optarg);
                break;
            case 'v': {
                use_video_file = true;
                video_file = std::string(optarg);
                break;
            }
            default: /* h */
                help();
                return EXIT_SUCCESS;
        }
    }

    /* ---------------- Post Process Values --------------- */
    if (remote_ip == "localhost") {
        remote_ip = "127.0.0.1";
    }
    
    if (use_camera && use_video_file) {
        LOGE("Video Camera (-c) and Video File (-v) can't both be enabled.");
        help();
        return EXIT_SUCCESS;
    }

    if (enable_depth && !use_camera) {
        LOGE("Depth estimation (-d) can only be enabled when the camera is also enabled (-c).");
        help();
        return EXIT_SUCCESS;
    }
    
    /* Notify user of used settings. */
    LOGI("ENGINE: Version %d", ENGINE_VERSION);
    summary(remote_ip, video_file, use_camera, use_video_file, enable_arduino, enable_depth);

    /* ---------------- Setup & Run System ---------------- */
    std::unique_ptr<ArduinoDriver> arduino_driver = nullptr;
    std::unique_ptr<FrameProvider> color_frame_provider = nullptr;
    std::unique_ptr<FrameProvider> depth_frame_provider = nullptr;
    std::unique_ptr<VideoTransmitter> depth_frame_transmitter   = nullptr;
    std::unique_ptr<VideoTransmitter> color_frame_transmitter   = nullptr;

    /* Setup & Start Arduino Driver. */
    if (enable_arduino) {
        arduino_driver = std::make_unique<ArduinoDriver>();
        arduino_driver->thread();
    }
    
    /* Setup & Start Frame Provider. */
    if (use_camera) {
        try {
            if (enable_depth) {
                struct stat buffer; 
                /**
                 * @note: Depending on the platfrom, the camera gets recognized as four or two video devices, with the depth video device corresponding to:
                 * - "/dev/video1" when only two video devices are found.
                 * - "/dev/video2" when four video devices are found.
                 */
                if (stat("/dev/video2", &buffer) == 0) { // the "/dev/video2" file exists
                    depth_frame_provider = std::make_unique<VideoCam>(VideoCam::CamType::MYNT_EYE_SINGLE, VideoCam::IO_Method::MMAP, "/dev/video2");
                    LOGI("Using the '/dev/video2' video device for depth stream.");
                } else {
                    LOGI("Using the '/dev/video1' video device for depth stream.");
                    depth_frame_provider = std::make_unique<VideoCam>(VideoCam::CamType::MYNT_EYE_SINGLE, VideoCam::IO_Method::MMAP, "/dev/video1");
                }
                
                depth_frame_provider->startStream();
            }
            color_frame_provider = std::make_unique<VideoCam>(VideoCam::CamType::MYNT_EYE_SINGLE, VideoCam::IO_Method::MMAP, "/dev/video0");
            color_frame_provider->startStream();
        } catch (const std::runtime_error& error) {
            LOGW("No camera device found, running without framegrabbers!");
            color_frame_provider = nullptr;
            depth_frame_provider = nullptr;
        }
    } else if (use_video_file) {
        color_frame_provider = std::make_unique<VideoFile>(video_file);
        color_frame_provider->startStream();
    }

    /* Setup Video Transmitters. */
    if (enable_depth) {
        depth_frame_transmitter = std::make_unique<VideoTransmitter>("udp://" + remote_ip + ":8998", depth_frame_provider.get());
        depth_frame_transmitter->thread();
    }

    color_frame_transmitter = std::make_unique<VideoTransmitter>("udp://" + remote_ip + ":8999", color_frame_provider.get());
    color_frame_transmitter->thread();

    /* Setup LAN connection. */
    robot::Remote remote = {2556, arduino_driver.get()};
    remote.connect();
    remote.start();

    /* Command threads to finnish. */
    remote.stop();
    if (arduino_driver) {
        arduino_driver->stop();
    }

    return EXIT_SUCCESS;
}
