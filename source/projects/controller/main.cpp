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
#include "remote/robot.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup Robot Communication. */
    remote::Robot robot = {"localhost", 2556};
    robot.connect();
    robot.thread();

    while (true) {
        int value = 0;
        std::cout << "INPUT: ";
        std::cin >> value;

        robot.sink({});
        __asm("");  // Avoid optimizing out.
    };
}

