cmake_minimum_required(VERSION 3.17)

# Define library target
# - INTERFACE: Does not reference any library or object files.
# - IMPORTED: Represents a pre-existing dependency.
# - GLOBAL: Target name globally visible.
add_library(ffmpeg INTERFACE IMPORTED GLOBAL)

# Find ffmpeg header directores and linker options
# https://cmake.org/cmake/help/latest/module/FindPkgConfig.html
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavdevice
    libavfilter
    libavformat
    libavcodec
    libpostproc
    libswresample
    libswscale
    libavutil
)

# Link the found lib
target_link_libraries(ffmpeg INTERFACE PkgConfig::LIBAV)

## ------------- PRE CMake 3.6 -------------
# # Specify header directories for libs
# target_link_directories(ffmpeg INTERFACE ${LIBAV_INCLUDE_DIRS})

# # Specify linker flags
# target_link_options(ffmpeg INTERFACE ${LIBAV_LDFLAGS})
