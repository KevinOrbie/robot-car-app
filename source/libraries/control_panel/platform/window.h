/**
 * @brief Window Abstraction.
 * @author Kevin Orbie
 */

#ifndef WINDOW_H
#define WINDOW_H

/* ========================== Include ========================== */
/* C/C++ Libraries */
#include <iostream>
#include <functional>

/* Thirdparty Libraries */
#include <glad/glad.h>  // Provides OpenGL header (needs to be included first)
#include <GLFW/glfw3.h>  // Windowing library

/* Custom C++ Libraries */
#include "common/input.h"
#include "common/logger.h"


/* ========================= Constants ========================= */
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;


/* =========================== Class =========================== */
/**
 * @brief Singleton Window class.
 * 
 * @note Making this class a singleton, makes it easy to deal with the 
 * c-style callbacks required by GLFW, which don't support method callbacks.
 * 
 * @note Don't forget to call 'cleanup()' to cleanup the glfw context on a different thread.
 */
class Window final{
   public:
    /**
     * @brief Singleton Window.
     * 
     * @note This object needs to be a singleton because of it's c-style callbacks.
     * @note This returns a pointer to allow classes to store a nullpointer.
     */
    static Window* instance() {
        static Window window;
        return &window;
    }

    void loop() {
        zeroCallbackOffsets(input_);
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    void updateInput() {
        /* Window-related Input & Actions */
        if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            close();

        /* Other Inputs */
        input_.keys[Button::W].updateState(glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS);
        input_.keys[Button::A].updateState(glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS);
        input_.keys[Button::S].updateState(glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS);
        input_.keys[Button::D].updateState(glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS);
        input_.keys[Button::LEFT ].updateState(glfwGetKey(window_, GLFW_KEY_LEFT ) == GLFW_PRESS);
        input_.keys[Button::RIGHT].updateState(glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS);
        input_.keys[Button::UP   ].updateState(glfwGetKey(window_, GLFW_KEY_UP   ) == GLFW_PRESS);
        input_.keys[Button::DOWN ].updateState(glfwGetKey(window_, GLFW_KEY_DOWN ) == GLFW_PRESS);
        input_.keys[Button::SPACE].updateState(glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS);
    }

    void cleanup() {
        glfwTerminate();
        running_ = false;
    }

    void close(){
        glfwSetWindowShouldClose(window_, true);
    }

    bool closing(){
        return glfwWindowShouldClose(window_);
    }

    void setTitle(char* title){
        glfwSetWindowTitle(window_, title);
    }

   private:
    Window() {
        /* Initialize Window */
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        /* Create a Window */
        window_ = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simple Cube", NULL, NULL);
        if (window_ == NULL)
        {
            LOGE("Failed to create GLFW window.");
            glfwTerminate();
            return;
        }

        /* Set OpenGL Context */
        glfwMakeContextCurrent(window_);

        /* Bind GLFW Callbacks */
        glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
        glfwSetCursorPosCallback(window_, mouse_callback);
        glfwSetScrollCallback(window_, scroll_callback);

        // Tell GLFW to capture our mouse
        // glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        /* Misc GLFW */
        glfwSwapInterval(0);  // Don't limit to 60 FPS.

        running_ = true;
    }

    ~Window() {
        cleanup();
    }

    /* ------------------- Input Handlding ------------------- */
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        Window *window_instance = Window::instance();
        window_instance->height_ = height;
        window_instance->width_  = width;
        glViewport(0, 0, width, height);
    }

    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
        static bool first_mouse = true;
        static float lastX = SCR_WIDTH / 2.0f;
        static float lastY = SCR_HEIGHT / 2.0f;

        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (first_mouse)
        {
            lastX = xpos;
            lastY = ypos;
            first_mouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        /* Updating input */
        Window *window_instance = Window::instance();
        window_instance->input_.mouse_xpos = xpos;
        window_instance->input_.mouse_ypos = xpos;
        window_instance->input_.mouse_xoffset = xoffset;
        window_instance->input_.mouse_yoffset = yoffset;
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        /* Updating input */
        Window *window_instance = Window::instance();
        window_instance->input_.scroll_x_offset = static_cast<float>(xoffset);
        window_instance->input_.scroll_y_offset = static_cast<float>(yoffset);
    }

    /* ------------------- Object Variables ------------------- */
   public:
    Input input_ = {};

    bool running_ = false;
    int height_ = SCR_HEIGHT;
    int width_  = SCR_WIDTH;

   private:
    GLFWwindow* window_ = nullptr;
};

#endif
