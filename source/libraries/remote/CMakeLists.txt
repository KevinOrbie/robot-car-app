set(LIB_NAME rca_remote)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Define Sources
list(APPEND SOURCE_FILES robot.cpp)
list(APPEND SOURCE_FILES system.cpp)

## Define Headers
list(APPEND HEADER_FILES robot.h)
list(APPEND HEADER_FILES system.h)


## --------------------------- Config ----------------------------
## Define the common library
add_library(${LIB_NAME} ${HEADER_FILES} ${SOURCE_FILES})

## Link External Libraries
target_link_libraries(${LIB_NAME} PRIVATE glad glfw stb_image glm ffmpeg v4l2 rca_common)

## Specify the root from which headers are defined
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
