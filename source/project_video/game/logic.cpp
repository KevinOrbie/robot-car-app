/**
 * @brief Defines all gamelogic.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */
#include "logic.h"

/* C/C++ Libraries */
#include <memory>

/* Third Party Libraries */
#include <glad/glad.h>                   // OpenGL function pointer library (needs to be included first)
#include <glm/glm.hpp>                   // GLSL Linear Algebra Library
#include <glm/gtc/matrix_transform.hpp>  // GLSL Linear Algebra Library
#include <glm/gtc/type_ptr.hpp>          // GLSL Linear Algebra Library

/* Custom Libraries */
#include "learnopengl/shader.h"
#include "learnopengl/camera.h"
#include "frame_provider.h"
#include "quad_screen.h"

/* ========================= Functions ========================= */
namespace game
{

struct GameState {
    int fps = 0;
    int fps_counter = 0;
    double fps_time = 0.0f;
    double time = 0.0f;

    std::unique_ptr<FrameProvider> frame_provider = nullptr;
    std::unique_ptr<QuadScreen> screen = nullptr;
    std::unique_ptr<Camera> camera  = nullptr;
    bool initialized = false;
};

static GameState gamestate;


int initialize() {
    /* Initialize GameState */
    gamestate.screen = std::make_unique<QuadScreen>();

    // Set Start Camera
    gamestate.camera = std::make_unique<Camera>(glm::vec3(0.0f, 3.0f, 2.0f));
    gamestate.camera->lookAt(0.0f, 0.0f, 0.0f);

    // Setup frame provider
    // gamestate.frame_provider = std::make_unique<VideoFile>("/home/kevin/Videos/normal-1080p.mp4");
    gamestate.frame_provider = std::make_unique<VideoFile>("/home/kevin/Videos/ultrawide-1440p.mp4");

    gamestate.initialized = true;

    /* Initialize OpenGL */
    glEnable(GL_DEPTH_TEST);

    return 0;
};

int destruct() {
    gamestate.screen.release();
    gamestate.camera.release();
    return 0;
};

int processFrame(float timedelta, int width, int height, Input& input) {
    if (!gamestate.initialized)
    { // Detect Wrong Order
        return 1;
    }

    /* FPS Counter */
    gamestate.time += timedelta;
    gamestate.fps_time += timedelta;
    gamestate.fps_counter++;
    if (gamestate.fps_time > 1.0f)
    {
        std::cout << "FPS: " << gamestate.fps_counter << "\n";
        gamestate.fps = gamestate.fps_counter;
        gamestate.fps_counter = 0;
        gamestate.fps_time -= 1.0f;
    }


    /* Process Input */
    if (input.keys[Button::W].held)
        gamestate.camera->ProcessKeyboard(FORWARD, timedelta);
    if (input.keys[Button::A].held)
        gamestate.camera->ProcessKeyboard(LEFT, timedelta);
    if (input.keys[Button::S].held)
        gamestate.camera->ProcessKeyboard(BACKWARD, timedelta);
    if (input.keys[Button::D].held)
        gamestate.camera->ProcessKeyboard(RIGHT, timedelta);
    if (input.mouse_xoffset != 0.0f || input.mouse_yoffset != 0.0f)
        gamestate.camera->ProcessMouseMovement(input.mouse_xoffset, input.mouse_yoffset);
    if (input.scroll_y_offset != 0.0f)
        gamestate.camera->ProcessMouseScroll(input.scroll_y_offset);
    // if (input.keys[Button::SPACE].pressed) {
    //     gamestate.grass->num_blades += 100000;
    //     std::cout << "Number of Blades: " << gamestate.grass->num_blades << "\n";
    // }

    /* Transformations */
    glm::mat4 view = gamestate.camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(gamestate.camera->Zoom), 
        (float)width / (float)height, 
        0.1f, 500.0f
    );

    /* Load Image */
    Frame new_frame = gamestate.frame_provider->getFrame(gamestate.time);
    gamestate.screen->load_texture(&new_frame.data[0], new_frame.width, new_frame.height, new_frame.channels, GL_RGB);

    /* Rendering */
    // Clear Screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Objects
    gamestate.screen->draw();
    return 0;
};

} // namespace game
