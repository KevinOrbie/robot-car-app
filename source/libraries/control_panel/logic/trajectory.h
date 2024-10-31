/**
 * @file trajectory.h
 * @author Kevin Orbie
 * @version 1.0
 * 
 * @brief OpenGL Trajectory Model Class.
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
 * @brief A 3D trajectory path.
 * 
 * NOTE: We can use glCopySubBuffer to only copy part of the buffer for every new element.
 */
class Trajectory {
   public:
    Trajectory(){
        /* Setting up vertex data. */
        vertices_ = {
            glm::vec3(0.0f, 0.01f, 0.0f),
            glm::vec3(0.5f, 0.01f, 0.0f),
            glm::vec3(0.5f, 0.01f, 0.5f),
            glm::vec3(2.0f, 0.01f, 2.0f)
        };
 
        /* Build / Compile Shader */
        shader_ = std::make_unique<Shader>("./shaders/trajectory.vs", "./shaders/trajectory.fs");

        /* Setting up Vertex Data Structures. */
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);

        // Load trajectory vertices into buffers.
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW); 

        // Position attribute.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);	

        /* Unset Bindings. */
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };

    ~Trajectory() {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
    }

    /**
     * @brief Run OpenGL draw calls.
     */
    void draw(glm::mat4 &view, glm::mat4 &projection){
        shader_->use();
        glBindVertexArray(VAO_);

        /* Update Uniform Values */
        shader_->setMat4("projection", projection);
        shader_->setMat4("view", view);

        /* Draw Grid Plane */
        glLineWidth(4.0);
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<unsigned int>(vertices_.size()));
        glLineWidth(1.0);
        glBindVertexArray(0);
    };

   private:
    unsigned int VAO_;
    unsigned int VBO_;
    std::unique_ptr<Shader> shader_;
    std::vector<glm::vec3> vertices_;
};
