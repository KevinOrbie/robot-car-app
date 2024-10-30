/**
 * @brief OpenGL QuadScreen Class.
 * @author Kevin Orbie
 */

#ifndef GRASS_H
#define GRASS_H

/* ========================== Include ========================== */
/* C/C++ Libraries */
#include <stdint.h>
#include <memory>

/* Third Party Libraries */
#include <stb_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>                   // GLSL Linear Algebra Library
#include <glm/gtc/matrix_transform.hpp>  // GLSL Linear Algebra Library
#include <glm/gtc/type_ptr.hpp>          // GLSL Linear Algebra Library

/* Custom c++ Libraries */
#include "common/logger.h"
#include "shader.h"


/* ========================== Classes ========================== */
class QuadScreen {
   public:
    QuadScreen() {
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

        /* Setting up Texture Data. */
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

    ~QuadScreen(){
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
    }

    void load_texture(uint8_t* data, int width, int height, GLenum format=GL_RGBA) {
        if (data) {   
            //std::cout << "Loading Texture: width, height, channels = " << width << ", " << height << ", " << channels << std::endl;
            glBindTexture(GL_TEXTURE_2D, texture_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            LOGW("Failed to load texture!");
        }
    }

    void draw() {
        shader_->use();

        glActiveTexture(GL_TEXTURE0);
        shader_->setInt("screenTexture", 0);
        glBindTexture(GL_TEXTURE_2D, texture_);

        glBindVertexArray(VAO_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

   private:
    std::unique_ptr<Shader> shader_  = nullptr;
    unsigned int texture_;
    unsigned int VAO_;
    unsigned int VBO_;
};

#endif
