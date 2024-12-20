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
#include "common/utils.h"  // gettid()


/* ========================== Classes ========================== */
ControlPanel::ControlPanel(Components components): 
    stopwatch_(Timer()), application_(std::make_unique<Application>(
        components.color_frame_provider,
        components.depth_frame_provider, 
        components.pose_provider,
        components.input_source, 
        components.input_sink
    )) 
{
    LOGI("Building Control Panel.");
};

/**
 * @note All OpenGL functions need to be called from the same thread where the OpenGL context is created.
 */
void ControlPanel::setup() {
    LOGI("Running Control Panel (TID = %d)", gettid());

    /* Create window. */
    window_ = Window::instance();

    /* Load all OpenGL function pointers */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOGE("Failed to initialize GLAD");
        throw std::runtime_error("Failed to initialize GLAD");
    }

    /* Setup GL State */
    application_->glsetup(); // Is this the best way of doing it?, maybe split OpenGL and other data, and initialize gamestate in ControlPanel constructor.

    /* Loop Preparations. */
    stopwatch_.start();
};

/**
 * @note All OpenGL functions need to be called from the same thread where the OpenGL context is created.
 */
void ControlPanel::cleanup() {
    application_->glcleanup();
    if (window_) {
        window_->cleanup();
    }
};

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
    user_input_.update(window_->input_);

    /* Logic & Rendering. */
    application_->processFrame(timedelta, window_->width_, window_->height_, user_input_);

    /* Draw Window. */
    window_->loop();
};
