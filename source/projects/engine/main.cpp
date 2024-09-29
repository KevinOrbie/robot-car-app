/**
 * @brief Main application running on the robot.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <iostream>
#include <thread>

/* Third Party Libraries */
// None

/* Custom C++ Includes */
#include "video/video_transmitter.h"
#include "video/video_cam.h"
#include "robot/arduino_driver.h"
#include "robot/remote.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup Arduino Driver. */
    // ArduinoDriver arduino = {};

    /* Setup LAN connection. */
    robot::Remote remote = {2556, nullptr};
    remote.connect();
    remote.thread();

    /* Start Arduino Driver. */
    // arduino.start();  // Run in this thread

    VideoCam camera = VideoCam();
    camera.startStream();

    VideoTransmitter transmitter = {"udp://127.0.0.1:8999", &camera};
    transmitter.start();

    /* Command threads to finnish. */
    remote.stop();
    return 0;
}
