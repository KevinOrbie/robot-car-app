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


/* ======================== Entry Point ======================== */
int main() {
    /* Create FrameProvider. */
    VideoFile frame_provider = VideoFile("/home/kevin/Videos/normal-1080p.mp4");

    /* Create GUI. */
    ControlPanel ctrlpanel = ControlPanel(&frame_provider);
    ctrlpanel.thread();

    // TODO: Update FrameProvider Classes (allow to copy the frame / pass by value), make ready for use on ROBOT.
    // TODO: Remove printf, and use throw instead of exit
    // TODO: Make ControlPanel work, given a File FrameProvider
    // TODO: Make ControlPanel work, given a Camera FrameProvider

    // TODO: Create ArduinoDriver (as an InputSink).
    // TODO: Think through what a command for the ArduinoDriver is (what Input / Control to pass around).
    // TODO: Make ControlPanel work, given an an InputSink.

    while (true) {__asm("");};
}

