cmake_minimum_required(VERSION 3.9)

# Define library target
# - INTERFACE: Does not reference any library or object files.
# - IMPORTED: Represents a pre-existing dependency.
# - GLOBAL: Target name globally visible.
add_library(v4l2 INTERFACE IMPORTED GLOBAL)

# Find v4l2 header directores and linker options
# https://cmake.org/cmake/help/latest/module/FindPkgConfig.html
find_package(PkgConfig REQUIRED)
pkg_check_modules(V4L2 REQUIRED IMPORTED_TARGET libv4l2)

# Link the found lib
target_link_libraries(v4l2 INTERFACE PkgConfig::V4L2)

# ------------- Alternative -------------
# Create a FindV4L.cmake file, and add the dirrectory to CMAKE_MODULE_PATH
# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
# find_package(V4L REQUIRED)


# # ================ TEST ================
add_executable(v4l2_compile_test capture.c)
target_link_libraries(v4l2_compile_test PRIVATE v4l2)
