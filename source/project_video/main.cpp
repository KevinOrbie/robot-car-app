/**
 * @brief Creating a grass render project.
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
#include "utils/timer.h"
#include "game/logic.h"

#include "game/video_cam.h"


/* ======================== Entry Point ======================== */
int main() {
    // // Setup custom objects
    // Window& window = Window::instance();
    // Timer stopwatch = Timer();

    // // Load all OpenGL function pointers
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    // {
    //     std::cout << "Failed to initialize GLAD" << std::endl;
    //     return -1;
    // }

    // // Setup Logic
    // game::initialize(); 

    // // Loop Preparations
    // stopwatch.start();

    // /* Render Loop */
    // while (!window.closing())
    // {
    //     // FPS Counter
    //     double timedelta = stopwatch.lap();

    //     // Input Update
    //     window.updateInput();

    //     // Logic & Rendering
    //     game::processFrame(timedelta, window.width_, window.height_, window.input_);

    //     // Draw Window
    //     window.loop();
    // }

    // game::destruct();

    VideoCam camera = VideoCam();
    camera.start();
    camera.getFrame(0.0);
    camera.getFrame(0.0);
    camera.stop();
}

