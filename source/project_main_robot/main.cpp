/**
 * @brief Main application running on the robot.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */
#include <inttypes.h>

/* Standard C++ Libraries */
#include <iostream>
#include <thread>
#include <vector>

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
    ArduinoSocket arduino_ctrl = ArduinoSocket();

    /* Simulate control loop. */
    while (true) {
        /* Recieve commands over LAN. */
        std::string recv_msg = server.recieve();
        if (!recv_msg.empty()) {
            // fprintf(stderr, "Recieved message: '%s'\n", recv_msg.c_str());
        }

        /* Process input. */
        int direction = 0;
        int throttle = 0;
        int speed = 255;

        switch (recv_msg[1]) {
            case 'L':
                direction = 1;
                break;

            case 'R':
                direction = 2;
                break;
            
            default:
                direction = 0;
                break;
        }

        switch (recv_msg[2]) {
            case 'U':
                throttle = 1;
                break;

            case 'D':
                throttle = 2;
                break;

            case 'B':
                throttle = 3;
                break;
            
            default:
                throttle = 0;
                break;
        }

        /* Build Arduino Command. */
        std::vector<uint8_t> command = {0};
        command[0] |= speed & 0x0F;
        command[0] |= (direction << 6);
        command[0] |= (throttle << 4);
        // fprintf(stderr, "Command: %x\n", static_cast<int>(command[0]));

        /* Command the Arduino. */
        arduino_ctrl.send(command);

        /* Slow down loop. */
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }

    return 0;
}

