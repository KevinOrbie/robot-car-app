/**
 * @brief Client Testing Application.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */

/* Standard C++ Libraries */
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

/* Third Party Libraries */
// None

/* Custom Includes */
#include "common/data_type.h"
#include "common/logger.h"
#include "common/client.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup LAN connection. */
    client::Client client("localhost", 2556, false);
    client.connect();

    /* Simulate control loop. */
    while (true) {
        /* Read user input. */
        float input = 0.0;
        std::cout << "INPUT: ";
        std::cin >> input;
        
        /* Send Message. */
        LOGI("Sending Message: '%f'", input);
        DriveControl payload = {};
        payload.speed = input;
        client.send(message::Message<message::MessageID::CMD_DRIVE>(payload));

        /* Recieve commands over LAN. */
        client.recieve();

        /* Slow down loop. */
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }

    return 0;
}
