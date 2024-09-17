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
#include "robot/arduino_driver.h"
#include "video/video_file.h"
#include "video/video_cam.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Create FrameProvider. */
    // VideoFile frame_provider = VideoFile("/home/kevin/Videos/normal-1080p.mp4");
    VideoCam frame_provider = VideoCam();
    frame_provider.start();

    /* Create Input Sink. */
    ArduinoDriver input_sink = ArduinoDriver();
    input_sink.thread();

    /* Create GUI. */
    ControlPanel ctrlpanel = ControlPanel(&frame_provider, &input_sink);
    ctrlpanel.thread();  // Run in seperate thread
    // ctrlpanel.start();  // Run in main thread.

    while (true) {
        // input_sink.iteration();
        __asm("");  // Avoid optimizing out.
    };
}

