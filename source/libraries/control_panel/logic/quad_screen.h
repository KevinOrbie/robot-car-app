/**
 * @brief OpenGL QuadScreen Class.
 * @author Kevin Orbie
 */

#pragma once

/* ========================== Include ========================== */
/* C/C++ Libraries */
#include <stdint.h>
#include <memory>
#include <vector>

/* Third Party Libraries */
#include <stb_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>                   // GLSL Linear Algebra Library
#include <glm/gtc/matrix_transform.hpp>  // GLSL Linear Algebra Library
#include <glm/gtc/type_ptr.hpp>          // GLSL Linear Algebra Library

/* Custom c++ Libraries */
#include "common/logger.h"
#include "texture.h"
#include "shader.h"


/* ========================== Classes ========================== */

/**
 * @brief A 3D screen that can display a 2D texture in 3D space.
 */
class QuadScreen {
   public:
    QuadScreen(std::shared_ptr<Texture> screen_texture=nullptr): texture_(screen_texture) {
        /* Setting up Vertex Data. */
        float vertices[] = {
            // Vertex Coords        // Texture Coords (inverted)
            -1.0f,  1.0f,  0.0f,    0.0f,  0.0f,
             1.0f,  1.0f,  0.0f,    1.0f,  0.0f,
            -1.0f, -1.0f,  0.0f,    0.0f,  1.0f,
             1.0f, -1.0f,  0.0f,    1.0f,  1.0f
        };

        /* Build / Compile Shader */
        shader_ = std::make_unique<Shader>("./shaders/quad_screen.vs", "./shaders/quad_screen.fs");

        /* Setting up Vertex Data Structures. */
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Tex Coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    ~QuadScreen() {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
    }

    /**
     * @brief Draw this screen quad to the HUD in the given location.
     * 
     * @param xpos: The amount of x pixels to the left wall in range [0, screen_width]
     * @param ypos: The amount of y pixels to the top wall in range [0, screen_height]
     * @param width: The amount of pixels the screen width should be.
     * @param height: The amount of pixels the screen height should be.
     * @param screen_width: Width of the total window screen.
     * @param screen_height: Height of the total window screen.
     */
    void draw(int xpos, int ypos, int width, int height, int screen_width, int screen_height) {
        /* Create model transform. */
        glm::mat4 model(1.0f);
        float scale_width = static_cast<float>(width) / static_cast<float>(screen_width);
        float scale_height = static_cast<float>(height) / static_cast<float>(screen_height);
        float norm_xpos = -1.0f + scale_width + static_cast<float>(xpos) / static_cast<float>(screen_width) * 2.0f;
        float norm_ypos =  1.0f - scale_height - static_cast<float>(ypos) / static_cast<float>(screen_height) * 2.0f;
        model = glm::translate(model, glm::vec3(norm_xpos, norm_ypos, 0.0f));
        model = glm::scale(model, glm::vec3(scale_width, scale_height, 1.0f));
        shader_->setMat4("model", model);

        glm::mat4 view = glm::mat4(1.0f);  // NOP
        glm::mat4 projection = glm::mat4(1.0f);  // NOP

        /* Draw with transform */
        draw(model, view, projection);
    }

    void draw(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection) {
        shader_->use();

        /* Apply model transform. */
        shader_->setMat4("model", model);
        shader_->setMat4("view", view);
        shader_->setMat4("projection", projection);

        /* Set texture. */
        if (texture_) {
            texture_->bind(0);
        }

        /* Draw quad triangles. */
        glBindVertexArray(VAO_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void draw() {
        draw(1, 1, 0, 0, 1, 1);
    }

   public:
    std::unique_ptr<Shader> shader_  = nullptr;
    std::shared_ptr<Texture> texture_;
    unsigned int VAO_;
    unsigned int VBO_;
};
