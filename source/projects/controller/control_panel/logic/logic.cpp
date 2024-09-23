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
#include "quad_screen.h"
#include "shader.h"
#include "camera.h"


/* ========================== Classes ========================== */
Application::Application(FrameProvider* frame_provider, InputSink *input_sink): 
        frame_provider_(frame_provider), input_sink_(input_sink) {
    /* Initialize Application State. */
    state = std::make_unique<AppState>();

    /* Setup Start Camera. */
    state->camera = std::make_unique<Camera>(glm::vec3(0.0f, 3.0f, 2.0f));
    state->camera->lookAt(0.0f, 0.0f, 0.0f);
};

void Application::glsetup() {
    LOGI("Initializing GL.");

    /* Initialize OpenGL Objects */
    state->screen = std::make_unique<QuadScreen>();

    /* Initialize OpenGL */
    glEnable(GL_DEPTH_TEST);

    state->opengl_initialized = true;
};

void Application::glcleanup() {
    /* Destory OpenGL Objects */
    state->screen.release();
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
        std::cout << "FPS: " << state->fps_counter << "\n";
        state->fps = state->fps_counter;
        state->fps_counter = 0;
        state->fps_time -= 1.0f;
    }

    /* Process Input */
    if (input.keys[Button::W].held) {
        state->camera->ProcessKeyboard(FORWARD, timedelta); 
        // LOGI("Key Updated: %s", (input.keysUpdated()) ? "true": "false");
    }
    if (input.keys[Button::A].held) {
        state->camera->ProcessKeyboard(LEFT, timedelta); 
        // LOGI("A held!");
    }
    if (input.keys[Button::S].held) {
        state->camera->ProcessKeyboard(BACKWARD, timedelta); 
        // LOGI("S held!");
    }
    if (input.keys[Button::D].held) {
        state->camera->ProcessKeyboard(RIGHT, timedelta); 
        // LOGI("D held!");
    }
    if (input.mouse_xoffset != 0.0f || input.mouse_yoffset != 0.0f)
        state->camera->ProcessMouseMovement(input.mouse_xoffset, input.mouse_yoffset);
    if (input.scroll_y_offset != 0.0f)
        state->camera->ProcessMouseScroll(input.scroll_y_offset);
    // if (input.keys[Button::SPACE].pressed) {
    //     grass->num_blades += 100000;
    //     std::cout << "Number of Blades: " << grass->num_blades << "\n";
    // }

    /* Forward Input to sink (optional). */
    if (input_sink_ && input.keysUpdated()) {
        input_sink_->sink(input);
    }

    /* Transformations */
    // glm::mat4 view = state->camera->GetViewMatrix();
    // glm::mat4 projection = glm::perspective(
    //     glm::radians(state->camera->Zoom), 
    //     (float)width / (float)height, 
    //     0.1f, 500.0f
    // );

    /* Load Image (optional) */
    if (frame_provider_) {
        Frame recieved_frame = frame_provider_->getFrame(state->time);
        Frame new_frame = Frame(recieved_frame, PixelFormat::YUV);
        state->screen->load_texture(
            &new_frame.data[0], 
            new_frame.width, 
            new_frame.height, 
            new_frame.channels, 
            GL_RGB
        );
    }

    /* Rendering */
    // Clear Screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Objects
    state->screen->draw();
    return true;
};
