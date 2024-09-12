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
#include "common/client.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup LAN connection. */
    client::Client client("localhost", 2556, false);
    client.connect();

    /* Simulate control loop. */
    while (true) {
        /* Read user input. */
        std::string input = "";
        while (input.empty() || (input[0] != 't' && input[0] != 'f')) {
            input = "";
            std::cin >> input;
        }
        
        /* Send Message. */
        bool value = (input == "true");
        LOGI("Sending Message: '%s'", input.c_str());
        client.send(message::Message<message::MessageID::CMD_DRIVE>(value));  // TODO: this line fails linking
        // I think this fails because the template is part of a library that is included, and thus, 
        // if this function is not used in the LIB code, then the template is not generated.
        // One solution could be to add a specialization in the library.

        /* Recieve commands over LAN. */
        client.recieve();

        /* Slow down loop. */
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }

    return 0;
}

// TODO: Make test project work as expected.
// TODO: Replace all fprintf with the correct logs.
// TODO: Remove perror and error function for logging.
// TODO: update Class Diagram.
// TODO: add Drive Controller & make control message work as intented.