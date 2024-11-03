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
#include <vector>

/* Third Party Libraries */
#include <glad/glad.h>
#include <glm/glm.hpp>                   // GLSL Linear Algebra Library
#include <glm/gtc/matrix_transform.hpp>  // GLSL Linear Algebra Library
#include <glm/gtc/type_ptr.hpp>          // GLSL Linear Algebra Library

/* Custom C++ Libraries */
#include "common/clock.h"
#include "common/pose.h"
#include "shader.h"
#include "utils.h"


/* ========================== Classes ========================== */

/**
 * @brief A 3D Model of the robot car.
 * 
 * @note tha coordinate system is oriented with: x-axis pointing to the front & y-axis pointing up.
 * 
 * TODO: To add direction, we could use three cubes, one for main car, one for camera, one for JN30B (but maybe add lighting effect, for better visibility).
 */
class CarModel {
   public:
    CarModel(PoseProvider *pose_provider): pose_provider_(pose_provider) {
        model_matrix_ = glm::mat4(1.0f);
        
        /* Setting up vertex data. */
        std::vector<glm::vec3> vertices = {
            glm::vec3( 0.1100f, 0.00f, -0.1025f),  // Bottom - left  - front
            glm::vec3( 0.1100f, 0.00f,  0.1025f),  // Bottom - right - front
            glm::vec3(-0.1100f, 0.00f, -0.1025f),  // Bottom - left  - back
            glm::vec3(-0.1100f, 0.00f,  0.1025f),  // Bottom - right - back
            glm::vec3( 0.1100f, 0.27f, -0.1025f),  // Top    - left  - front
            glm::vec3( 0.1100f, 0.27f,  0.1025f),  // Top    - right - front
            glm::vec3(-0.1100f, 0.27f, -0.1025f),  // Top    - left  - back
            glm::vec3(-0.1100f, 0.27f,  0.1025f)   // Top    - right - back
        };

        std::vector<unsigned int> indices = {
            1, 2, 0, // Bottom
            1, 2, 3, // Bottom
            5, 6, 4, // Top
            5, 6, 7, // Top
            4, 2, 0, // Left
            4, 2, 6, // Left
            5, 3, 1, // Right
            5, 3, 7, // Right
            4, 1, 5, // Front
            4, 1, 0, // Front
            6, 3, 2, // Back
            6, 3, 7  // Back
        };
 
        /* Build / Compile Shader */
        shader_ = std::make_unique<Shader>("./shaders/car.vs", "./shaders/car.fs");

        /* Setting up Vertex Data Structures. */
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &EBO_);  
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);

        // Load trajectory vertices into buffers.
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW); 

        // Load indexes to form cube.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);  
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Position attribute.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);	

        /* Unset Bindings. */
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };

    ~CarModel() {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
        glDeleteBuffers(1, &EBO_);
    };

    void update() {
        if (pose_provider_) {
            Pose pose = pose_provider_->getPose(common::now());
            // model_matrix_ = glm::mat4(1.0f);
            // model_matrix_ = glm::translate(model_matrix_, glm::vec3(pose.x(), pose.y(), pose.z()));
            model_matrix_ = utils::convert(pose.toMatrix());
        }
    }

    /**
     * @brief Run OpenGL draw calls.
     */
    void draw(glm::mat4 &view, glm::mat4 &projection){
        shader_->use();
        glBindVertexArray(VAO_);

        /* Update Uniform Values */
        shader_->setMat4("projection", projection);
        shader_->setMat4("model", model_matrix_);
        shader_->setMat4("view", view);

        /* Draw Grid Plane */
        glDrawElements(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };

   private:
    unsigned int VAO_;
    unsigned int VBO_;
    unsigned int EBO_;
    glm::mat4 model_matrix_;
    PoseProvider *pose_provider_;
    std::unique_ptr<Shader> shader_;
};
