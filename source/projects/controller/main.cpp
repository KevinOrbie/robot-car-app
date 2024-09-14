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

/* Custom Libraries */
#include "control_panel/control_panel.h"


/* ======================== Entry Point ======================== */
int main() {
    ControlPanel ctrlpanel = ControlPanel();
    ctrlpanel.thread();

    while (true) {__asm("");};
}

