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
#include "robot/arduino_driver.h"
#include "robot/remote.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup Arduino Driver. */
    ArduinoDriver arduino = {};

    /* Setup LAN connection. */
    robot::Remote remote = {2556, &arduino};
    remote.connect();
    remote.thread();

    /* Start Arduino Driver. */
    // arduino.start();  // Run in this thread

    while (true) { 
        __asm(""); 
    }; // Avoid optimizing out.
    return 0;
}

// TODO: Compile on Jetson
// TODO: Test Sending messages between Jetson & Desktop.