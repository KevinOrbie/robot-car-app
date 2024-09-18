/**
 * @brief Main application running on the robot.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */
/* Standard C++ Libraries */
#include <iostream>
#include <thread>

/* Third Party Libraries */
// None

/* Custom C++ Includes */
#include "robot/remote.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup LAN connection. */
    robot::Remote remote = {2556};
    remote.thread();

    while (true) { __asm(""); }; // Avoid optimizing out.
    return 0;
}

