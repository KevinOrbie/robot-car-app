/**
 * @brief OpenGL Texture Class.
 * @author Kevin Orbie
 */

#pragma once

/* ========================== Include ========================== */
/* C/C++ Libraries */
#include <cassert>

/* Third Party Libraries */
#include <stb_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>                   // GLSL Linear Algebra Library
#include <glm/gtc/matrix_transform.hpp>  // GLSL Linear Algebra Library
#include <glm/gtc/type_ptr.hpp>          // GLSL Linear Algebra Library

/* Custom c++ Libraries */
#include "common/logger.h"


/* ========================== Classes ========================== */

/**
 * @brief A texture to be used by OpenGL.
 * @note Making this a seperate class, allows the same texture to be used by multiple classes.
 */
class Texture {
   public:
    Texture() {
        /* Setting up Texture Data. */
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    /* Rule of five. */
    ~Texture() {
        glDeleteTextures(1, &texture_);
    };

    Texture(Texture &other) = delete;  // Prevent the creation of duplicate textures.
    Texture(Texture &&other) = default; 
    Texture& operator=(Texture &other) = delete;
    Texture& operator=(Texture &&other) = default;

    void load(uint8_t* data, int width, int height, GLenum format=GL_RGBA) {
        if (data) {   
            //std::cout << "Loading Texture: width, height, channels = " << width << ", " << height << ", " << channels << std::endl;
            glBindTexture(GL_TEXTURE_2D, texture_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            width_ = width;
            height_ = height;
        } else {
            LOGW("No texture data given, failed to load texture!");
        }
    }

    void bind(const int texture_unit_id=0) {
        assert(texture_unit_id >= 0);

        /* There are a limited amount of texture units. */
        int max_texture_units = 0;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
        assert(texture_unit_id < max_texture_units);

        glActiveTexture(GL_TEXTURE0 + texture_unit_id);
        glBindTexture(GL_TEXTURE_2D, texture_);
    }

    int getWidth() { return width_; };
    int getHeight() { return height_; };

   private:
    unsigned int texture_;
    int width_ = 0;
    int height_ = 0;
};
