/**
 * @file logic.h
 * @author Kevin Orbie
 * 
 * @brief Defines all game logic.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <memory>

/* Custom C++ Libraries */
#include "video/frame_provider.h"
#include "common/input.h"
#include "common/input_sink.h"
#include "camera.h"

/* Custom OpenGL Objects */
#include "quad_screen.h"
#include "grid.h"


/* ========================== Classes ========================== */
/**
 * @brief Originally this was meant to hide the implementation details from the Application class. 
 * This would allow use not to have to include the Application Object definitions in this header.
 * 
 * But it is not possible to neatly use a unique pointer, without the class being fully defined.
 * Leaving it as is for now.
 */
struct AppState {
    /* Internal State Variables */
    int fps = 0;
    int fps_counter = 0;
    double fps_time = 0.0f;
    double time = 0.0f;
    std::unique_ptr<Camera> camera  = nullptr;
    
    /* OpenGL Variables */
    std::unique_ptr<QuadScreen> screen = nullptr;
    std::unique_ptr<ShaderGrid2D> grid = nullptr;
    bool opengl_initialized = false;
};

class Application {
   public:
    Application(FrameProvider *frame_provider=nullptr, InputSink *input_sink=nullptr);

    void glsetup();
    void glcleanup();

    bool processFrame(float timedelta, int width, int height, Input& input);

   private:
    std::unique_ptr<AppState> state = nullptr;
    FrameProvider *frame_provider_  = nullptr;
    InputSink *input_sink_          = nullptr;
};
