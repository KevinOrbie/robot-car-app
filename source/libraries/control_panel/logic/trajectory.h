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
#include <array>
#include <memory>
#include <algorithm>

/* Third Party Libraries */
#include <glad/glad.h>
#include <glm/glm.hpp>                   // GLSL Linear Algebra Library
#include <glm/gtc/matrix_transform.hpp>  // GLSL Linear Algebra Library
#include <glm/gtc/type_ptr.hpp>          // GLSL Linear Algebra Library

/* Custom C++ Libraries */
#include "common/logger.h"
#include "common/clock.h"
#include "shader.h"


/* ========================== Classes ========================== */

/**
 * @brief A 3D trajectory path.
 */
class Trajectory {
   public:
    Trajectory(PoseProvider *pose_provider): pose_provider_(pose_provider){
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

    void update() {
        position_t position = pose_provider_->getPose(common::now()).getPosition();
        addPosition(position[0], position[1], position[2]);
    }

    void addPosition(float x, float y, float z) {
        if (vertices_.size() < point_limit_) {
            vertices_.push_back(glm::vec3(x,y,z));
        } else {
            /* Shift all vector elements to the left by 1. */
            std::move(vertices_.begin() + 1, vertices_.end(), vertices_.begin());
            vertices_.back() = glm::vec3(x,y,z);
        }
        sendDataToGPU();
        LOGI("Number of points: %d", vertices_.size());
    }

    /**
     * @brief Sends the current object's vertex data to the GPU. 
     * 
     * @note Even with 100,000 points, we still get 3800 FPS, thus, for the moment this is not a terribly bad bottleneck.
     * @note If this would become a bad bottleneck, we could opt to go for an indexed drawing method, and only send the new index and point.
     */
    void sendDataToGPU() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    PoseProvider *pose_provider_;
    std::unique_ptr<Shader> shader_;

    std::vector<glm::vec3> vertices_ = {};
    int point_limit_ = 100000;
};
