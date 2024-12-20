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
#include "common/input_source.h"
#include "common/input_sink.h"
#include "common/input.h"
#include "common/pose.h"
#include "camera.h"

/* Custom OpenGL Objects */
#include "depth_image_cloud.h"
#include "quad_screen.h"
#include "trajectory.h"
#include "texture.h"
#include "frustum.h"
#include "grid.h"
#include "car.h"


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
    bool camera_follow = false;
    
    /* OpenGL Variables */
    std::shared_ptr<Texture> depth_video = nullptr;
    std::shared_ptr<Texture> color_video = nullptr;

    std::unique_ptr<CarModel> car = nullptr;
    std::unique_ptr<Frustum> frustum = nullptr;
    std::unique_ptr<QuadScreen> screen = nullptr;
    std::unique_ptr<ShaderGrid2D> grid = nullptr;
    std::unique_ptr<Trajectory> trajectory = nullptr;
    std::unique_ptr<DepthImageCloud> depth_cloud = nullptr;
    bool opengl_initialized = false;
};


class Application {
   public:
    Application(
        FrameProvider *color_frame_provider=nullptr, 
        FrameProvider *depth_frame_provider=nullptr, 
        PoseProvider *pose_provider=nullptr,
        InputSource *input_source=nullptr,
        InputSink *input_sink=nullptr
    );

    void glsetup();
    void glcleanup();

    bool processFrame(float timedelta, int width, int height, Input& input);

   private:
    std::unique_ptr<AppState> state = nullptr;
    FrameProvider *color_frame_provider_  = nullptr;
    FrameProvider *depth_frame_provider_  = nullptr;
    PoseProvider *pose_provider_    = nullptr;
    InputSource *input_source_      = nullptr;
    InputSink *input_sink_          = nullptr;
};
