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
#include "shader.h"


/* ========================== Classes ========================== */

/**
 * @brief A 3D screen that can display a 2D texture in 3D space.
 */
class DepthImageCloud {
   public:
    DepthImageCloud() {
        /* Setting up GPU instanced Vertex Data. */
        float vertex[] = { 0.0f,  0.0f,  0.0f };

        /* Build / Compile Shader */
        shader_ = std::make_unique<Shader>("./shaders/depth_image_cloud.vs", "./shaders/depth_image_cloud.fs");

        /* Setting up Texture Data. */
        glGenTextures(1, &depth_texture_);
        glBindTexture(GL_TEXTURE_2D, depth_texture_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        /* Setting up Vertex Data Structures. */
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        /* Unset Bindings. */
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~DepthImageCloud() {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
    }

    void load_texture(uint8_t* data, int width, int height, GLenum format=GL_RGBA) {
        image_width_ = width;
        image_height_ = height;

        if (data) {   
            //std::cout << "Loading Texture: width, height, channels = " << width << ", " << height << ", " << channels << std::endl;
            glBindTexture(GL_TEXTURE_2D, depth_texture_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            LOGW("Failed to load texture!");
        }
    }

    void draw(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection) {
        shader_->use();

        /* Apply transforms. */
        shader_->setMat4("model", model);
        shader_->setMat4("view", view);
        shader_->setMat4("projection", projection);

        /* Set texture. */
        shader_->setInt("image_width", image_width_);
        shader_->setInt("image_height", image_height_);
        shader_->setInt("plane_width", image_width_);
        shader_->setInt("plane_height", image_height_);

        glActiveTexture(GL_TEXTURE0);
        shader_->setInt("depthTexture", 0);
        glBindTexture(GL_TEXTURE_2D, depth_texture_);

        /* Draw quad triangles. */
        glBindVertexArray(VAO_);
        glDrawArraysInstanced(GL_POINTS, 0, 1, image_width_ * image_height_);
    }

   private:
    std::unique_ptr<Shader> shader_  = nullptr;
    unsigned int depth_texture_;
    unsigned int color_texture_;
    unsigned int VAO_;
    unsigned int VBO_;

    int image_width_ = 0;
    int image_height_ = 0;

    const float baseline_ = 0.120f;
    const float focus_length_ = 0.00245f;
    const float view_angle_vertical_ = 58.0f;
    const float view_angle_horizontal_ = 105.0f;
};
