/**
 * @brief A project that runs the system entirely locally, without any communication.
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
#include "robot/arduino_driver.h"
#include "video/video_cam.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup Arduino Driver. */
    ArduinoDriver arduino = {};
    arduino.thread();  // Run in this thread

    /* Setup Camera. */
    VideoCam camera = VideoCam();
    camera.startStream();

    /* Setup Controller. */
    ControlPanel panel = {&camera, &arduino};
    panel.start();

    /* Stop all loopers. */
    arduino.stop();
    return 0;
}

