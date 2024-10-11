/**
 * @brief Main application running on the robot.
 * @author Kevin Orbie
 */

#define ENGINE_VERSION 0

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <unistd.h>  // getopt

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
    msg += "  -d              enable the arduino driver\n";
    msg += "  -c              stream from the camera\n";
    msg += "  -v <path>       stream from the video file\n";
    msg += "  -i <address>    ip address of the remote to connect to\n";
    
    msg += "\n";

    fprintf(stderr, "%s", msg.c_str());
};

static void summary(std::string remote_ip, std::string video_file, bool use_camera, bool use_video_file, bool enable_arduino) {
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
    LOGI("  > Frame Provider : %s", frame_provider.c_str());
    LOGI("  > Input Sink     : %s", input_sink.c_str());
    LOGI("  > Remote IP      : %s", remote_ip.c_str());
    LOGI("---------------------------");
};

int main(int argc, char *argv[]) {
    /* ------------------ Default Values ------------------ */
    std::string remote_ip = "192.168.0.234";
    std::string video_file;

    bool use_camera     = false;
    bool use_video_file = false;
    bool enable_arduino = false;

    /* ----------------- Parse User Input ----------------- */
    int option;
    while ((option = getopt(argc, argv, "acv:di:h")) != -1) {
        switch (option) {
            case 'a': {
                use_camera = true;
                enable_arduino = true;
                break;
            }
            case 'c':
                use_camera = true;
                break;
            case 'd':
                enable_arduino = true;
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
    
    /* Notify user of used settings. */
    LOGI("ENGINE: Version %d", ENGINE_VERSION);
    summary(remote_ip, video_file, use_camera, use_video_file, enable_arduino);

    /* ---------------- Setup & Run System ---------------- */
    std::unique_ptr<ArduinoDriver> arduino        = nullptr;
    std::unique_ptr<FrameProvider> frame_provider = nullptr;

    /* Setup & Start Arduino Driver. */
    if (enable_arduino) {
        arduino = std::make_unique<ArduinoDriver>();
        arduino->thread();
    }
    
    /* Setup & Start Frame Provider. */
    if (use_camera) {
        frame_provider = std::make_unique<VideoCam>();
        frame_provider->startStream();
    } else if (use_video_file) {
        frame_provider = std::make_unique<VideoFile>(video_file);
        frame_provider->startStream();
    }

    /* Setup LAN connection. */
    robot::Remote remote = {2556, arduino.get()};
    remote.connect();
    remote.thread();

    /* Setup Video Transmitter. */
    VideoTransmitter transmitter = {"udp://" + remote_ip + ":8999", frame_provider.get()};
    transmitter.start();  // Run in local thread.

    /* Command threads to finnish. */
    remote.stop();
    if (arduino) {
        arduino->stop();
    }

    return EXIT_SUCCESS;
}
