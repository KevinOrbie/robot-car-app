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
#include "video/video_file.h"
#include "video/video_cam.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Create FrameProvider. */
    // VideoFile frame_provider = VideoFile("/home/kevin/Videos/normal-1080p.mp4");
    VideoCam frame_provider = VideoCam();
    frame_provider.start();

    /* Create GUI. */
    ControlPanel ctrlpanel = ControlPanel(&frame_provider);
    ctrlpanel.thread();  // Run in seperate thread
    // ctrlpanel.start();  // Run in main thread.

    // TODO: Create ArduinoDriver (as an InputSink).
    // TODO: Think through what a command for the ArduinoDriver is (what Input / Control to pass around).
    // TODO: Make ControlPanel work, given an an InputSink.

    while (true) {__asm("");};
}

