/**
 * @brief GUI applaction used to control the robot remotely.
 * @author Kevin Orbie
 */

#define CONTROLLER_VERSION 0

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <unistd.h>  // getopt
#include <sys/stat.h>  // stat

/* Standard C++ Libraries */
#include <stdexcept>
#include <iostream>
#include <thread>
#include <vector>

/* Custom C++ Libraries */
#include "navigation/trajectory_follower.h"
#include "control_panel/control_panel.h"
#include "robot/robot_simulation.h"
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
    msg += "  -m              enable the arduino driver (don't require remote connection)\n";
    msg += "  -d              enable the depth estimation (experimental)\n";
    msg += "  -c              stream from the camera\n";
    msg += "  -t              standalone test configuration\n";
    msg += "  -v <path>       stream from the video file\n";
    msg += "  -i <address>    ip address of the robot to connect to\n";
    
    msg += "\n";

    fprintf(stderr, "%s", msg.c_str());
};

static void summary(std::string robot_ip, std::string video_file, bool use_camera, bool use_video_file, bool enable_arduino, bool test_mode, bool enable_depth) {
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
    LOGI("  > Frame Provider   : %s", frame_provider.c_str());
    LOGI("  > Depth Estimation : %s", (enable_depth) ? "enabled":"disabled");
    LOGI("  > Input Sink       : %s", input_sink.c_str());
    LOGI("  > Robot IP         : %s", robot_ip.c_str());
    LOGI("---------------------------");
};

int main(int argc, char *argv[]) {
    /* ------------------ Default Values ------------------ */
    std::string robot_ip = "192.168.0.212";
    std::string video_file;

    bool test_mode      = false;
    bool use_camera     = false;
    bool enable_depth   = false;
    bool use_video_file = false;
    bool enable_arduino = false;

    /* ----------------- Parse User Input ----------------- */
    int option;
    while ((option = getopt(argc, argv, "actv:mdi:h")) != -1) {
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
            case 'm':
                enable_arduino = true;
                break;
            case 'd':
                enable_depth = true;
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

    if (enable_depth && !use_camera) {
        LOGE("Depth estimation (-d) can only be enabled when the camera is also enabled (-c).");
        help();
        return EXIT_SUCCESS;
    }

    if (test_mode) {
        robot_ip = "none";
        enable_arduino = false;
    }
    
    /* Notify user of used settings. */
    LOGI("CONTROLLER: Version %d", CONTROLLER_VERSION);
    summary(robot_ip, video_file, use_camera, use_video_file, enable_arduino, test_mode, enable_depth);

    /* ---------------- Setup & Run System ---------------- */
    std::unique_ptr<FrameProvider> color_frame_provider = nullptr;
    std::unique_ptr<FrameProvider> depth_frame_provider = nullptr;
    std::unique_ptr<PoseProvider> pose_provider   = nullptr;

    std::unique_ptr<InputSinkSplitter> input_sink = std::make_unique<InputSinkSplitter>();
    std::unique_ptr<robot::RobotInputSimulation> simulation = std::make_unique<robot::RobotInputSimulation>();
    std::unique_ptr<ArduinoDriver> arduino_driver = nullptr;
    std::unique_ptr<remote::Robot> robot = nullptr;

    NodeTrajectory trajectory = {{{0, 0, 0}, {0.9, 0, 0}, {0.9, 0, -0.6}, {0, 0, -0.6}}, false};
    std::unique_ptr<TrajectoryFollower> trajectory_follower = std::make_unique<TrajectoryFollower>(simulation.get(), trajectory);

    /* Setup & Start Input Sink. */
    input_sink->add(simulation.get());
    if (enable_arduino) {
        arduino_driver = std::make_unique<ArduinoDriver>();
        input_sink->add(arduino_driver.get());
        arduino_driver->thread();
    } else if(!test_mode) {
        robot = std::make_unique<remote::Robot>(robot_ip, 2556);
        input_sink->add(robot.get());
        robot->connect();
        robot->thread();
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
                } else {
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
    } else if (test_mode) {
        color_frame_provider = nullptr;
    } else {
        if (enable_depth) {
            depth_frame_provider = std::make_unique<VideoReciever>("udp://" + robot_ip + ":8998");
            depth_frame_provider->startStream();
            dynamic_cast<VideoReciever*>(depth_frame_provider.get())->thread();
        }
        color_frame_provider = std::make_unique<VideoReciever>("udp://" + robot_ip + ":8999");
        color_frame_provider->startStream();
        dynamic_cast<VideoReciever*>(color_frame_provider.get())->thread();
    }

    /* Setup & Start Controller. */
    ControlPanel::Components panel_components = {};
    panel_components.color_frame_provider = color_frame_provider.get();
    panel_components.depth_frame_provider = depth_frame_provider.get();
    panel_components.pose_provider = simulation.get();
    panel_components.input_source = trajectory_follower.get();
    panel_components.input_sink = input_sink.get();
    ControlPanel panel = {panel_components};
    panel.start();  // Run in main thread

    /* Command threads to finnish. */
    if (enable_arduino) {
        dynamic_cast<ArduinoDriver*>(input_sink.get())->stop();
    } else {
        dynamic_cast<remote::Robot*>(input_sink.get())->stop();
    }

    if (!use_camera && !use_video_file) {
        if (enable_depth) {
            dynamic_cast<VideoReciever*>(depth_frame_provider.get())->stop();
        }
        dynamic_cast<VideoReciever*>(color_frame_provider.get())->stop();
    }

    return 0;
}

