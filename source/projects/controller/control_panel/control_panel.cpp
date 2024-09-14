/**
 * @file control_panel.h
 * @author Kevin Orbie
 * 
 * @brief Defines an interface to the GUI System.
 */

/* ========================== Include ========================== */
#include "control_panel.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <stdexcept>
#include <memory>

/* Custom C++ Libraries */
#include "video/frame_provider.h"
#include "common/input_sink.h"
#include "common/logger.h"
#include "common/looper.h"


/* ========================== Classes ========================== */
ControlPanel::ControlPanel(FrameProvider *frame_provider, InputSink *input_sink): stopwatch_(Timer()) {
    LOGI("Building Control Panel.");
};

/**
 * @note All OpenGL functions need to be called from the same thread where the OpenGL context is created.
 */
void ControlPanel::setup() {
    LOGI("Starting Control Panel.");

    /* Create window. */
    window_ = Window::instance();

    /* Load all OpenGL function pointers */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOGE("Failed to initialize GLAD");
        throw std::runtime_error("Failed to initialize GLAD");
    }

    /* Setup Logic */
    game::initialize(); 

    /* Loop Preparations. */
    stopwatch_.start();
}

/**
 * @note All OpenGL functions need to be called from the same thread where the OpenGL context is created.
 */
void ControlPanel::cleanup() {
    game::destruct();
    if (window_) {
        window_->cleanup();
    }
}

/**
 * @brief Render Loop iteration.
 */
void ControlPanel::iteration() {
    /* Check if the user closed the window. */
    if (window_->closing()) { stop(); return; }

    /* FPS Counter. */
    double timedelta = stopwatch_.lap();

    /* Input Update. */
    window_->updateInput();

    /* Logic & Rendering. */
    game::processFrame(timedelta, window_->width_, window_->height_, window_->input_);

    /* Draw Window. */
    window_->loop();
};
