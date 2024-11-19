/**
 * @brief OpenGL Frustum Class.
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
 * @brief A 3D frustrum wireframe.
 * 
 * @note The model coordinate system has the camera centered at the origin, 
 * and the Frustrum facing in the X-axis, with the Z-axis pointing to the right.
 */
class Frustum {
   public:
    Frustum(float near, float far, float angle_horiz_deg, float angle_verti_deg, std::shared_ptr<Texture> screen_texture=nullptr): texture_(screen_texture) {
        /* Setting up Vertex Data. */
        float far_plane_half_h = far * glm::tan(glm::radians(angle_horiz_deg/2.0f));
        float far_plane_half_v = far * glm::tan(glm::radians(angle_verti_deg/2.0f));
        float vertices[] = {
            // Vertex Coords 
             0.0f,  0.0f,  0.0f,  // Camera Center
             far,  far_plane_half_v,  far_plane_half_h,
             far,  far_plane_half_v, -far_plane_half_h,
             far, -far_plane_half_v, -far_plane_half_h,
             far, -far_plane_half_v,  far_plane_half_h,
             far,  far_plane_half_v,  far_plane_half_h  // Go to first point again in FAN
        };

        /* Build / Compile Shader */
        shader_ = std::make_unique<Shader>("./shaders/frustum.vs", "./shaders/frustum.fs");

        /* Setting up Vertex Data Structures. */
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    ~Frustum() {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
    }

    void draw(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection) {
        shader_->use();

        /* Apply model transform. */
        shader_->setMat4("projection", projection);
        shader_->setMat4("model", model);
        shader_->setMat4("view", view);

        /* Set texture. */
        if (texture_) {
            texture_->bind(0);
        }

        /* Draw quad triangles. */
        glBindVertexArray(VAO_);
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glDrawArrays(GL_TRIANGLE_FAN, 0, 18);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

   public:
    std::unique_ptr<Shader> shader_  = nullptr;
    std::shared_ptr<Texture> texture_;
    unsigned int VAO_;
    unsigned int VBO_;
};
