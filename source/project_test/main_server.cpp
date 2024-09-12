/**
 * @brief Server Testing Application.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */

/* Standard C++ Libraries */
#include <thread>
#include <chrono>

/* Third Party Libraries */
// None

/* Custom C++ Includes */
#include "common/server.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup LAN connection. */
    server::Server server(2556, false);
    server.connect();

    /* Simulate control loop. */
    while (true) {
        /* Recieve commands over LAN. */
        server.recieve();

        /* Slow down loop. */
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}
