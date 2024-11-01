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
Application::Application(FrameProvider* frame_provider, InputSink *input_sink, PoseProvider *pose_provider): 
        frame_provider_(frame_provider), input_sink_(input_sink), pose_provider_(pose_provider) {
    /* Initialize Application State. */
    state = std::make_unique<AppState>();

    /* Setup Start Camera. */
    state->camera = std::make_unique<Camera>(glm::vec3(0.0f, 3.0f, 2.0f));
    state->camera->lookAt(0.0f, 0.0f, 0.0f);
};

void Application::glsetup() {
    LOGI("Initializing GL.");

    /* Initialize OpenGL Objects */
    state->trajectory = std::make_unique<Trajectory>(pose_provider_);
    state->screen = std::make_unique<QuadScreen>();
    state->grid = std::make_unique<ShaderGrid2D>();
    state->car = std::make_unique<CarModel>(pose_provider_);

    /* Initialize OpenGL */
    glEnable(GL_DEPTH_TEST);

    state->opengl_initialized = true;
};

void Application::glcleanup() {
    /* Destory OpenGL Objects */
    state->trajectory.release();
    state->screen.release();
    state->grid.release();
    state->car.release();
};

bool Application::processFrame(float timedelta, int width, int height, Input& input) {
    /* Make sure OpenGL is initialized. */
    if (!state->opengl_initialized) {
        return false;
    }

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
    // if (input.keys[Button::SPACE].held) {
    //     static int count = 0;
    //     state->trajectory->addPosition(static_cast<float>(count) * 0.02, 0.0f, static_cast<float>(count) * 0.02);
    //     count++;
    // }

    /* Forward Input to sink (optional). */
    if (input_sink_ && input.keysUpdated()) {
        input_sink_->sink(input);
    }

    /* Load Image (optional) */
    if (frame_provider_) {
        Frame new_frame = frame_provider_->getFrame(state->time, PixelFormat::YUV);
        state->screen->load_texture(
            new_frame.image.getData(), 
            new_frame.image.getWidth(), 
            new_frame.image.getHeight(), 
            GL_RGB
        );
    }

    /* Transformations */
    glm::mat4 view = state->camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(state->camera->Zoom), 
        (float)width / (float)height, 
        0.1f, 500.0f
    );

    /* Update Objects. */
    state->car->update();

    /* Rendering */
    // Clear Screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Objects
    state->screen->draw(10, 10, 16 * 30, 9 * 30, width, height);
    state->trajectory->draw(view, projection);
    state->grid->draw(view, projection);
    state->car->draw(view, projection);
    return true;
};
