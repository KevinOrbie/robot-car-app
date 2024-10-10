/**
 * @brief GUI applaction used to control the robot remotely.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <iostream>
#include <thread>

/* Custom C++ Libraries */
#include "control_panel/control_panel.h"
#include "video/video_reciever.h"
#include "remote/robot.h"

#include "video/video_file.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup Robot Communication. */
    // remote::Robot robot = {"192.168.0.212", 2556};
    remote::Robot robot = {"localhost", 2556};
    robot.connect();
    robot.thread();

    /* Setup VideoReciever. */
    // VideoReciever frame_grabber = VideoReciever("udp://127.0.0.1:8999");
    // VideoReciever frame_grabber = VideoReciever("udp://192.168.0.212:8999");  // To the Server
    // frame_grabber.thread();
    VideoFile frame_grabber = VideoFile("/home/kevin/Videos/normal-1080p.mp4");
    frame_grabber.startStream();

    /* Setup Controller. */
    ControlPanel panel = {&frame_grabber, &robot};
    panel.start();

    /* Command threads to finnish. */
    // frame_grabber.stop();
    robot.stop();
    return 0;
}

