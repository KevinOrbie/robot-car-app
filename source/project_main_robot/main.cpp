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

/* Custom Includes */
#include "common/server.h"
#include "arduino_socket.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup LAN connection. */
    Server server = Server(2556, false);
    server.link();

    /* Setup Arduino connection. */
    // ArduinoSocket arduino_ctrl = ArduinoSocket();

    /* Simulate control loop. */
    while (true) {
        /* Recieve commands over LAN. */
        std::string recv_msg = server.recieve();
        if (!recv_msg.empty()) {
            fprintf(stderr, "Recieved message: '%s'\n", recv_msg.c_str());
        } else {
            // fprintf(stderr, "Nothing to recieve.\n");
        }

        /* Read user input. */
        // std::string x;
        // std::cout << "Type a char: "; // Type a number and press enter
        // std::cin >> x;

        /* Command the Arduino. */
        // arduino_ctrl.send("???");

        // std::string rec_msg = "";
        // arduino_ctrl.recieve(rec_msg);
        // fprintf(stderr, "Recieved message: %s\n", rec_msg.c_str());

        /* Slow down loop. */
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }

    return 0;
}

