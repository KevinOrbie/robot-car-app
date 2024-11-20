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
    CarModel() {
        model_matrix_ = glm::mat4(1.0f);

        const float car_half_length = 0.1100f;
        const float car_half_width = 0.1025f;
        const float car_height = 0.2700f;
        
        /* Setting up vertex data. */
        const std::vector<glm::vec3> vertices = {
            /* Bottom Face: */
            glm::vec3( car_half_length, 0.0000000f,  car_half_width), glm::vec3( 0.0f, -1.0f,  0.0f), 
            glm::vec3(-car_half_length, 0.0000000f, -car_half_width), glm::vec3( 0.0f, -1.0f,  0.0f), 
            glm::vec3( car_half_length, 0.0000000f, -car_half_width), glm::vec3( 0.0f, -1.0f,  0.0f), 
            glm::vec3( car_half_length, 0.0000000f,  car_half_width), glm::vec3( 0.0f, -1.0f,  0.0f), 
            glm::vec3(-car_half_length, 0.0000000f, -car_half_width), glm::vec3( 0.0f, -1.0f,  0.0f), 
            glm::vec3(-car_half_length, 0.0000000f,  car_half_width), glm::vec3( 0.0f, -1.0f,  0.0f), 

            /* Top Face: */
            glm::vec3( car_half_length, car_height,  car_half_width), glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3(-car_half_length, car_height, -car_half_width), glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( car_half_length, car_height, -car_half_width), glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( car_half_length, car_height,  car_half_width), glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3(-car_half_length, car_height, -car_half_width), glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3(-car_half_length, car_height,  car_half_width), glm::vec3( 0.0f,  1.0f,  0.0f),

            /* Left Face: */
            glm::vec3( car_half_length, car_height, -car_half_width), glm::vec3( 0.0f,  0.0f, -1.0f), 
            glm::vec3(-car_half_length, 0.0000000f, -car_half_width), glm::vec3( 0.0f,  0.0f, -1.0f), 
            glm::vec3( car_half_length, 0.0000000f, -car_half_width), glm::vec3( 0.0f,  0.0f, -1.0f), 
            glm::vec3( car_half_length, car_height, -car_half_width), glm::vec3( 0.0f,  0.0f, -1.0f), 
            glm::vec3(-car_half_length, 0.0000000f, -car_half_width), glm::vec3( 0.0f,  0.0f, -1.0f), 
            glm::vec3(-car_half_length, car_height, -car_half_width), glm::vec3( 0.0f,  0.0f, -1.0f), 

            /* Right Face: */
            glm::vec3( car_half_length, car_height,  car_half_width), glm::vec3( 0.0f,  0.0f,  1.0f), 
            glm::vec3(-car_half_length, 0.0000000f,  car_half_width), glm::vec3( 0.0f,  0.0f,  1.0f), 
            glm::vec3( car_half_length, 0.0000000f,  car_half_width), glm::vec3( 0.0f,  0.0f,  1.0f), 
            glm::vec3( car_half_length, car_height,  car_half_width), glm::vec3( 0.0f,  0.0f,  1.0f), 
            glm::vec3(-car_half_length, 0.0000000f,  car_half_width), glm::vec3( 0.0f,  0.0f,  1.0f), 
            glm::vec3(-car_half_length, car_height,  car_half_width), glm::vec3( 0.0f,  0.0f,  1.0f), 

            /* Front Face: */
            glm::vec3( car_half_length, car_height, -car_half_width), glm::vec3( 1.0f,  0.0f,  0.0f), 
            glm::vec3( car_half_length, 0.0000000f,  car_half_width), glm::vec3( 1.0f,  0.0f,  0.0f), 
            glm::vec3( car_half_length, car_height,  car_half_width), glm::vec3( 1.0f,  0.0f,  0.0f), 
            glm::vec3( car_half_length, car_height, -car_half_width), glm::vec3( 1.0f,  0.0f,  0.0f), 
            glm::vec3( car_half_length, 0.0000000f,  car_half_width), glm::vec3( 1.0f,  0.0f,  0.0f), 
            glm::vec3( car_half_length, 0.0000000f, -car_half_width), glm::vec3( 1.0f,  0.0f,  0.0f), 

            /* Back Face: */
            glm::vec3(-car_half_length, car_height, -car_half_width), glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-car_half_length, 0.0000000f,  car_half_width), glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-car_half_length, 0.0000000f, -car_half_width), glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-car_half_length, car_height, -car_half_width), glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-car_half_length, 0.0000000f,  car_half_width), glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-car_half_length, car_height,  car_half_width), glm::vec3(-1.0f,  0.0f,  0.0f) 
        };
 
        /* Build / Compile Shader */
        shader_ = std::make_unique<Shader>("./shaders/car.vs", "./shaders/car.fs");

        /* Setting up Vertex Data Structures. */
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);

        // Load all data vertices into buffer.
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW); 

        // Position attribute (refers to bound VBO) 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute (refers to bound VBO) 
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);	

        /* Unset Bindings. */
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };

    ~CarModel() {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
    };

    void position(Pose pose) {
        model_matrix_ = utils::convert(pose.toMatrix());
    }

    /**
     * @brief Run OpenGL draw calls.
     */
    void draw(glm::mat4 &view, glm::mat4 &projection){
        shader_->use();
        glBindVertexArray(VAO_);

        /* Normal Matrix */
        /* Link: https://learnopengl.com/Lighting/Basic-Lighting */
        glm::mat3 normal = glm::transpose(glm::inverse(model_matrix_));

        /* Update Uniform Values */
        shader_->setMat4("projection", projection);
        shader_->setMat4("model", model_matrix_);
        shader_->setMat3("normal", normal);
        shader_->setMat4("view", view);

        /* Draw Grid Plane */
        glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(24 * 2));
        glBindVertexArray(0);
    };

   private:
    unsigned int VAO_;
    unsigned int VBO_;
    glm::mat4 model_matrix_;
    std::unique_ptr<Shader> shader_;
};
