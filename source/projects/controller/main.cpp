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
#include "remote/robot.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup Robot Communication. */
    // remote::Robot robot = {"192.168.0.231", 2556};
    remote::Robot robot = {"localhost", 2556};
    robot.connect();
    robot.thread();

    /* Setup Controller. */
    ControlPanel panel = {nullptr, &robot};
    panel.start();

    // while (true) {
    //     int value = 0;
    //     std::cout << "INPUT: ";
    //     std::cin >> value;

    //     robot.sink({});
    //     __asm("");  // Avoid optimizing out.
    // };
}

