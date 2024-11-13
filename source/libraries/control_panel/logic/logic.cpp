/**
 * @file logic.cpp
 * @author Kevin Orbie
 * 
 * @brief Defines all game logic.
 */

/* ========================== Include ========================== */
#include "logic.h"

/* Standard C++ Libraries */
#include <memory>

/* Third Party Libraries */
#include <glad/glad.h>                   // OpenGL function pointer library (needs to be included first)
#include <glm/glm.hpp>                   // GLSL Linear Algebra Library
#include <glm/gtc/matrix_transform.hpp>  // GLSL Linear Algebra Library
#include <glm/gtc/type_ptr.hpp>          // GLSL Linear Algebra Library

/* Custom C++ Libraries */
#include "video/frame_provider.h"
#include "common/logger.h"

/* Custom C++ Source Code */
#include "shader.h"
#include "camera.h"


/* ========================== Classes ========================== */
Application::Application(
        FrameProvider *color_frame_provider, 
        FrameProvider *depth_frame_provider, 
        PoseProvider *pose_provider,
        InputSource *input_source,
        InputSink *input_sink
    ): 
        color_frame_provider_(color_frame_provider), 
        depth_frame_provider_(depth_frame_provider), 
        pose_provider_(pose_provider), 
        input_source_(input_source),
        input_sink_(input_sink)
{
    /* Initialize Application State. */
    state = std::make_unique<AppState>();

    /* Setup Start Camera. */
    state->camera = std::make_unique<Camera>(glm::vec3(-2.0f, 2.0f, 0.0f));
    state->camera->lookAt(0.0f, 0.0f, 0.0f);
};

void Application::glsetup() {
    LOGI("Initializing GL.");

    /* Initialize OpenGL Objects */
    state->depth_cloud = std::make_unique<DepthImageCloud>();
    state->trajectory = std::make_unique<Trajectory>(pose_provider_);
    state->screen = std::make_unique<QuadScreen>();
    state->grid = std::make_unique<ShaderGrid2D>();
    state->car = std::make_unique<CarModel>();

    /* Initialize OpenGL */
    glEnable(GL_DEPTH_TEST);

    state->opengl_initialized = true;
};

void Application::glcleanup() {
    /* Destory OpenGL Objects */
    state->depth_cloud.release();
    state->trajectory.release();
    state->screen.release();
    state->grid.release();
    state->car.release();
};

bool Application::processFrame(float timedelta, int width, int height, Input& user_input) {
    /* Make sure OpenGL is initialized. */
    if (!state->opengl_initialized) {
        return false;
    }

    /* Get Variables */
    Pose pose_WO = pose_provider_->getPose(common::now()); // Car Object Frame w.r.t. World Frame

    /* FPS Counter */
    state->time += timedelta;
    state->fps_time += timedelta;
    state->fps_counter++;
    if (state->fps_time > 1.0f) {
        LOGI("FPS: %d", state->fps_counter);
        state->fps = state->fps_counter;
        state->fps_counter = 0;
        state->fps_time -= 1.0f;
    }

    /* Merge Inputs */
    Input input = user_input;
    static bool prev_control_by_user = true;
    if (input_source_) {
        /* Use control drive input, if no user drive input. */
        if (
            !user_input.keys[Button::UP].held && !user_input.keys[Button::UP].updated &&
            !user_input.keys[Button::LEFT].held && !user_input.keys[Button::LEFT].updated &&
            !user_input.keys[Button::DOWN].held && !user_input.keys[Button::DOWN].updated &&
            !user_input.keys[Button::RIGHT].held && !user_input.keys[Button::RIGHT].updated
        ) {
            Input control_input = input_source_->getInput();
            
            input.keys[Button::UP] = control_input.keys[Button::UP];
            input.keys[Button::LEFT] = control_input.keys[Button::LEFT];
            input.keys[Button::DOWN] = control_input.keys[Button::DOWN];
            input.keys[Button::RIGHT] = control_input.keys[Button::RIGHT];

            if (prev_control_by_user){
                /* This just sets one random button to be updated, to make sure that the sink gets this (possibly) the new input. */
                input.keys[Button::UP].updated = true;
                prev_control_by_user = false;
            }
        } else {
            prev_control_by_user = true;
        }

        // NOTE: When simpy changing to the the button, this not set the updated flag to True, and thus, any changes here won't be tranferred to the sink
        // NOTE: The current implementation using a static variable is a simple bootstrap, but does not account for the Input struct design issue, do we even need a updated flag for each button?
    }
    
    /* Process Input */
    if (input.keys[Button::W].held) {
        state->camera->ProcessKeyboard(FORWARD, timedelta); 
    }
    if (input.keys[Button::A].held) {
        state->camera->ProcessKeyboard(LEFT, timedelta); 
    }
    if (input.keys[Button::S].held) {
        state->camera->ProcessKeyboard(BACKWARD, timedelta); 
    }
    if (input.keys[Button::D].held) {
        state->camera->ProcessKeyboard(RIGHT, timedelta); 
    }
    if (input.mouse_xoffset != 0.0f || input.mouse_yoffset != 0.0f) {
        state->camera->ProcessMouseMovement(input.mouse_xoffset, input.mouse_yoffset);
    }
    if (input.scroll_y_offset != 0.0f) {
        state->camera->ProcessMouseScroll(input.scroll_y_offset);
    }
    
    if (input.keys[Button::T].pressed) {
        state->camera->viewFrom(0.0f, 5.0f, 0.0f);
        state->camera->lookAt(0.0f, 0.0f, 0.0f);
        // TODO: Maybe fix camera to move in a plane?
    }

    if (input.keys[Button::F].pressed) {
        state->camera_follow = !state->camera_follow;
    }

    /* Update Trajectory. */
    static double timer = 0.0;
    timer += timedelta;
    if (timer > 0.3) {  // Update every 0.5 sec
        state->trajectory->update();
        timer -= 0.3;
    }

    /* Forward Input to sink (optional). */
    if (input_sink_ && input.keysUpdated()) {
        input_sink_->sink(input);
    }

    /* Load Image (optional) */
    if (depth_frame_provider_) {
        // NOTE: Depth image must be loaded first
        Frame new_frame = depth_frame_provider_->getFrame(state->time, PixelFormat::YUV);
        state->depth_cloud->load_texture(
            new_frame.image.getData(), 
            new_frame.image.getWidth(), 
            new_frame.image.getHeight(), 
            GL_RGB
        );
    }

    if (color_frame_provider_) {
        Frame new_frame = color_frame_provider_->getFrame(state->time, PixelFormat::YUV);
        state->screen->load_texture(
            new_frame.image.getData(), 
            new_frame.image.getWidth(), 
            new_frame.image.getHeight(), 
            GL_RGB
        );
    }

    /* (optional) Update Follow Camera */
    if (state->camera_follow) {
        const position_t pos_OC_O = {-2.0, 3.0, 0.0};
        const Pose pose_OC = Pose(pos_OC_O);
        position_t pos_WO_W = pose_WO.getPosition();
        position_t pos_WC_W = (pose_WO * pose_OC).getPosition();
        state->camera->viewFrom(pos_WC_W[0], pos_WC_W[1], pos_WC_W[2]);
        state->camera->lookAt(pos_WO_W[0], pos_WO_W[1], pos_WO_W[2]);
    }
    
    /* Transformations */
    glm::mat4 view = state->camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(state->camera->Zoom), 
        (float)width / (float)height, 
        0.1f, 500.0f
    );

    /* Update Objects. */
    state->car->position(pose_WO);

    position_t camera_offset_OC_O_ = {0.08f, 0.25f, 0.0f};
    Pose pose_OC = Pose(camera_offset_OC_O_); // Camera Frame w.r.t. Car Object Frame
    glm::mat4 cloud_model = utils::convert((pose_WO * pose_OC).toMatrix());  // Camera Frame w.r.t. World Frame

    /* Rendering */
    // Clear Screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Objects
    state->depth_cloud->draw(cloud_model, view, projection, state->screen->texture_);
    state->screen->draw(10, 10, 16 * 30, 9 * 30, width, height);
    state->trajectory->draw(view, projection);
    state->grid->draw(view, projection);
    state->car->draw(view, projection);
    return true;
};
