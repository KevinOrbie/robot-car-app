/**
 * @brief Main application running on the robot.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */
/* C/C++ Libraries */
#include <iostream>
#include <thread>

/* Third Party Libraries */
// None

/* Custom Libraries */
#include "common/server.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup LAN connection. */
    Server server = Server(2556, false);
    server.link();

    /* Setup Arduino connection. */
    // TODO: fill in

    /* Simulate control loop. */
    while (true) {
        /* Recieve commands over LAN. */
        std::string recv_msg = server.recieve();
        if (!recv_msg.empty()) {
            fprintf(stderr, "Recieved message: '%s'\n", recv_msg.c_str());
        } else {
            fprintf(stderr, "Nothing to recieve.\n");
        }

        /* Command the Arduino. */
        // TODO: fill in

        /* Slow down loop. */
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}

