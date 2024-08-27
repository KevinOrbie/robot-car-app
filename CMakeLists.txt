cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(OpenGLTests LANGUAGES C CXX VERSION 1.0)

# ## ---------------- Find external packages ----------------
# NOTE: Working with already build versions of these libs, allows us to skip building them over and over again.

# [GLFW] Platform-independent API for creating windows, contexts and surfaces, reading input, handling events, etc.
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/thirdparty/glfw/_deploy/)
find_package(glfw3 3.4 REQUIRED)

# [FFmpeg] record, convert and stream audio and video
add_subdirectory(thirdparty/ffmpeg)

# [GLM] OpenGL Mathematics library
# Add glm as part of the project (header-only)
add_subdirectory(thirdparty/glm)

# Created a custom static lib from the source code
add_subdirectory(thirdparty/glad)           # [GLAD] GL loader-generator
add_subdirectory(thirdparty/stb_image)      # [STB_image] Image library


## ------------------- Call other code --------------------
add_subdirectory(source)

