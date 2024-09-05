/**
 * @brief GUI applaction used to control the robot remotely.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */
/* C/C++ Libraries */
#include <iostream>
#include <thread>

/* Third Party Libraries */
// None

/* Custom Libraries */
#include "platform/window.h"
#include "common/client.h"
#include "utils/timer.h"
#include "game/logic.h"

#include "game/video_cam.h"


/* ======================== Entry Point ======================== */
int main() {
    // Setup custom objects
    Window& window = Window::instance();
    Timer stopwatch = Timer();

    // Setup connection
    Client client = Client("localhost", 2556, false);
    client.link();

    // Load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Logic
    game::initialize(); 

    // Loop Preparations
    stopwatch.start();

    /* Render Loop */
    while (!window.closing())
    {
        // FPS Counter
        double timedelta = stopwatch.lap();

        // Input Update
        window.updateInput();

        // Send input to server
        std::string input_msg = "[xxxx]";
        input_msg[1] = (window.input_.keys[Button::LEFT ].held) ? 'L' : 'x';
        input_msg[2] = (window.input_.keys[Button::RIGHT].held) ? 'R' : 'x';
        input_msg[3] = (window.input_.keys[Button::UP   ].held) ? 'U' : 'x';
        input_msg[4] = (window.input_.keys[Button::DOWN ].held) ? 'D' : 'x';
        if (input_msg != "[xxxx]"){
            client.send(input_msg);
        }

        // Logic & Rendering
        game::processFrame(timedelta, window.width_, window.height_, window.input_);

        // Draw Window
        window.loop();
    }

    game::destruct();
}

