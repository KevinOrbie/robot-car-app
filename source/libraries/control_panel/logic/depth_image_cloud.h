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
class DepthImageCloud {
   public:
    DepthImageCloud(std::shared_ptr<Texture> depth_texture, std::shared_ptr<Texture> color_texture=nullptr): depth_texture_(depth_texture), color_texture_(color_texture) {
        /* Setting up GPU instanced Vertex Data. */
        float vertex[] = { 0.0f,  0.0f,  0.0f };

        /* Build / Compile Shader */
        shader_ = std::make_unique<Shader>("./shaders/depth_image_cloud.vs", "./shaders/depth_image_cloud.fs");

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

    void draw(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection) {
        shader_->use();

        /* Apply transforms. */
        shader_->setMat4("model", model);
        shader_->setMat4("view", view);
        shader_->setMat4("projection", projection);

        /* Set texture. */
        shader_->setInt("image_width", depth_texture_->getWidth());
        shader_->setInt("image_height", depth_texture_->getHeight());

        if (depth_texture_) {
            shader_->setInt("depthTexture", 0);
            depth_texture_->bind(0);
        }

        if (color_texture_) {
            shader_->setInt("colorTexture", 1);
            color_texture_->bind(1);
        }

        /* Draw quad triangles. */
        glBindVertexArray(VAO_);
        glPointSize(3);
        glDrawArraysInstanced(GL_POINTS, 0, 1, depth_texture_->getWidth() * depth_texture_->getHeight());
    }

   private:
    std::unique_ptr<Shader> shader_  = nullptr;
    std::shared_ptr<Texture> depth_texture_;
    std::shared_ptr<Texture> color_texture_;
    unsigned int VAO_;
    unsigned int VBO_;

    const float baseline_ = 0.120f;
    const float focus_length_ = 0.00245f;
    const float view_angle_vertical_ = 58.0f;
    const float view_angle_horizontal_ = 105.0f;
};
