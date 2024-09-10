/**
 * @brief Main application running on the robot.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */

/* Standard C++ Libraries */

/* Third Party Libraries */
// None

/* Custom Includes */
#include "common/server.h"

/* ======================== Entry Point ======================== */
int main() {
    /* Setup LAN connection. */
    Server server = Server(2556, false);
    server.link();

    /* Simulate control loop. */
    while (true) {
        /* Recieve commands over LAN. */
        std::string recv_msg = server.recieve();
        if (!recv_msg.empty()) {
            // fprintf(stderr, "Recieved message: '%s'\n", recv_msg.c_str());
        }

        /* Slow down loop. */
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }

    return 0;
}

