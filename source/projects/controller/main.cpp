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


/* ======================== Entry Point ======================== */
int main() {
    /* Setup Robot Communication. */
    remote::Robot robot = {"192.168.0.231", 2556};
    // remote::Robot robot = {"localhost", 2556};
    robot.connect();
    robot.thread();

    /* Setup VideoReciever. */
    // VideoReciever reciever = VideoReciever("udp://127.0.0.1:8999");
    VideoReciever reciever = VideoReciever("udp://192.168.0.231:8999");  // To the Server
    reciever.thread();

    /* Setup Controller. */
    ControlPanel panel = {&reciever, &robot};
    panel.start();

    /* Command threads to finnish. */
    reciever.stop();
    robot.stop();
    return 0;
}

