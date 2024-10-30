/**
 * @brief GUI applaction used to control the robot remotely.
 * @author Kevin Orbie
 */

#define CONTROLLER_VERSION 0

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <unistd.h>  // getopt

/* Standard C++ Libraries */
#include <stdexcept>
#include <iostream>
#include <thread>

/* Custom C++ Libraries */
#include "control_panel/control_panel.h"
#include "video/video_reciever.h"
#include "remote/robot.h"

#include "robot/arduino_driver.h"
#include "video/video_file.h"
#include "video/video_cam.h"


/* ======================== Entry Point ======================== */
static void help() {
    std::string msg = "";

    /* Usage. */
    msg += "usage: controller [options]\n";

    /* Explanation. */
    msg += "\nOptions:\n";
    msg += "  -h              display this help message\n";
    msg += "  -a              enable camera and arduino driver\n";
    msg += "  -d              enable the arduino driver (don't require remote connection)\n";
    msg += "  -c              stream from the camera\n";
    msg += "  -t              standalone test configuration\n";
    msg += "  -v <path>       stream from the video file\n";
    msg += "  -i <address>    ip address of the robot to connect to\n";
    
    msg += "\n";

    fprintf(stderr, "%s", msg.c_str());
};

static void summary(std::string robot_ip, std::string video_file, bool use_camera, bool use_video_file, bool enable_arduino, bool test_mode) {
    std::string frame_provider = "";
    if (use_camera) {
        frame_provider = "camera";
    } else if (use_video_file) {
        frame_provider = video_file;
    } else if (test_mode) {
        frame_provider = "none";
    } else {
        frame_provider = "robot";
    }

    std::string input_sink = "";
    if (enable_arduino) {
        input_sink = "arduino";
    } else if (test_mode) {
        input_sink = "none";
    } else {
        input_sink = "robot";
    }

    LOGI("--------- Summary ---------");
    LOGI("  > Frame Provider : %s", frame_provider.c_str());
    LOGI("  > Input Sink     : %s", input_sink.c_str());
    LOGI("  > Robot IP       : %s", robot_ip.c_str());
    LOGI("---------------------------");
};

int main(int argc, char *argv[]) {
    /* ------------------ Default Values ------------------ */
    std::string robot_ip = "192.168.0.212";
    std::string video_file;

    bool test_mode      = false;
    bool use_camera     = false;
    bool use_video_file = false;
    bool enable_arduino = false;

    /* ----------------- Parse User Input ----------------- */
    int option;
    while ((option = getopt(argc, argv, "actv:di:h")) != -1) {
        switch (option) {
            case 'a': {
                use_camera = true;
                enable_arduino = true;
                robot_ip = "localhost";
                break;
            }
            case 't':
                test_mode = true;
                break;
            case 'c':
                use_camera = true;
                break;
            case 'd':
                enable_arduino = true;
                break;
            case 'i':
                robot_ip = std::string(optarg);
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
    if (robot_ip == "localhost") {
        robot_ip = "127.0.0.1";
    }
    
    if (use_camera && use_video_file) {
        LOGE("Video Camera (-c) and Video File (-v) can't both be enabled.");
        help();
        return EXIT_SUCCESS;
    }

    if (test_mode) {
        robot_ip = "none";
        use_camera = false;
        enable_arduino = false;
    }
    
    /* Notify user of used settings. */
    LOGI("CONTROLLER: Version %d", CONTROLLER_VERSION);
    summary(robot_ip, video_file, use_camera, use_video_file, enable_arduino, test_mode);

    /* ---------------- Setup & Run System ---------------- */
    std::unique_ptr<FrameProvider> frame_provider = nullptr;
    std::unique_ptr<InputSink> input_sink         = nullptr;

    /* Setup & Start Input Sink. */
    if (enable_arduino) {
        input_sink = std::make_unique<ArduinoDriver>();
        dynamic_cast<ArduinoDriver*>(input_sink.get())->thread();
    } else if(test_mode) {
        input_sink = nullptr;
    } else {
        input_sink = std::make_unique<remote::Robot>(robot_ip, 2556);
        dynamic_cast<remote::Robot*>(input_sink.get())->connect();
        dynamic_cast<remote::Robot*>(input_sink.get())->thread();
    }

    /* Setup & Start Frame Provider. */
    if (use_camera) {
        try {
            frame_provider = std::make_unique<VideoCam>();
            frame_provider->startStream();
        } catch (const std::runtime_error& error) {
            LOGW("No camera device found, running without framegrabber!");
            frame_provider = nullptr;
        }
    } else if (use_video_file) {
        frame_provider = std::make_unique<VideoFile>(video_file);
        frame_provider->startStream();
    } else if (test_mode) {
        frame_provider = nullptr;
    } else {
        frame_provider = std::make_unique<VideoReciever>("udp://" + robot_ip + ":8999");
        frame_provider->startStream();
        dynamic_cast<VideoReciever*>(frame_provider.get())->thread();
    }

    /* Setup & Start Controller. */
    ControlPanel panel = {frame_provider.get(), input_sink.get()};
    panel.start();  // Run in main thread

    /* Command threads to finnish. */
    if (enable_arduino) {
        dynamic_cast<ArduinoDriver*>(input_sink.get())->stop();
    } else {
        dynamic_cast<remote::Robot*>(input_sink.get())->stop();
    }

    if (!use_camera && !use_video_file) {
        dynamic_cast<VideoReciever*>(frame_provider.get())->stop();
    }

    return 0;
}

