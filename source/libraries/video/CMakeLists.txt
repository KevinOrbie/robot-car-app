set(LIB_NAME rca_video)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Define Sources
list(APPEND SOURCE_FILES video_transmitter.cpp)
list(APPEND SOURCE_FILES image_conversion.cpp)
list(APPEND SOURCE_FILES video_reciever.cpp)
list(APPEND SOURCE_FILES video_file.cpp)
list(APPEND SOURCE_FILES video_cam.cpp)
list(APPEND SOURCE_FILES image.cpp)

## Define Headers
list(APPEND HEADER_FILES video_transmitter.h)
list(APPEND HEADER_FILES frame_provider.h)
list(APPEND HEADER_FILES video_reciever.h)
list(APPEND HEADER_FILES video_file.h)
list(APPEND HEADER_FILES video_cam.h)
list(APPEND HEADER_FILES image.h)


## --------------------------- Config ----------------------------
## Define the common library
add_library(${LIB_NAME} ${HEADER_FILES} ${SOURCE_FILES})

## Specify the root from which headers are defined
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)

## Link External Libraries
target_link_libraries(${LIB_NAME} PUBLIC ffmpeg)
target_link_libraries(${LIB_NAME} PRIVATE v4l2 rca_common)