/**
 * @brief OpenGL Grid Model Class.
 * @author Kevin Orbie
 * @version 1.0
 */

#pragma once

/* ========================== Include ========================== */
/* C/C++ Libraries */
#include <memory>
#include <array>

/* Third Party Libraries */
#include <glad/glad.h>
#include <glm/glm.hpp>                   // GLSL Linear Algebra Library
#include <glm/gtc/matrix_transform.hpp>  // GLSL Linear Algebra Library
#include <glm/gtc/type_ptr.hpp>          // GLSL Linear Algebra Library

/* Custom C++ Libraries */
#include "shader.h"


/* ========================== Classes ========================== */

/**
 * @brief A 2D grid, but the actual grid lines are drawn in the Shader.
 */
class ShaderGrid2D {
   public:
    ShaderGrid2D(){
        /* Setting up vertex data. */
        /* Note: Vectices that define plane to draw 3D Grid on. */
        vertices_ = {
            glm::vec3(x_range_[0], 0.0f, z_range_[0]),
            glm::vec3(x_range_[0], 0.0f, z_range_[1]),
            glm::vec3(x_range_[1], 0.0f, z_range_[0]),
            glm::vec3(x_range_[1], 0.0f, z_range_[1])
        };
 
        /* Build / Compile Shader */
        shader_ = std::make_unique<Shader>("./shaders/grid.vs", "./shaders/grid.fs");

        /* Setting up Vertex Data Structures. */
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);

        // Load plane verteces into buffers.
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW); 

        // Position attribute.
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        /* Unset VAO. */
        glBindVertexArray(0);
    };

    ~ShaderGrid2D() {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
    }

    /**
     * @brief Run OpenGL draw calls.
     */
    void draw(glm::mat4 &view, glm::mat4 &projection){
        shader_->use();
        glBindVertexArray(VAO_);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Disable Wireframe mode

        /* Update Uniform Values */
        shader_->setFloat("tick_distance", tick_distance_);
        shader_->setFloat("linewidth", linewidth_);
        shader_->setMat4("projection", projection);
        shader_->setMat4("view", view);

        /* Draw Grid Plane */
        glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<unsigned int>(vertices_.size()));
        glBindVertexArray(0);
    };

   private:
    unsigned int VAO_;
    unsigned int VBO_;
    std::unique_ptr<Shader> shader_;
    std::vector<glm::vec3> vertices_;

    std::array<float, 2> x_range_ = {-100.0f, 100.0f};
    std::array<float, 2> z_range_ = {-100.0f, 100.0f};
    float tick_distance_ = 1.0f;
    float linewidth_ = 0.008f;
};
