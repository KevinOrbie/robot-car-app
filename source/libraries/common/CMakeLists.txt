set(LIB_NAME rca_common)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Define Sources
list(APPEND SOURCE_FILES logger.cpp)

## Define Headers
list(APPEND HEADER_FILES input_source.h)
list(APPEND HEADER_FILES input_sink.h)
list(APPEND HEADER_FILES looper.h)
list(APPEND HEADER_FILES logger.h)
list(APPEND HEADER_FILES input.h)
list(APPEND HEADER_FILES timer.h)
list(APPEND HEADER_FILES clock.h)
list(APPEND HEADER_FILES utils.h)
list(APPEND HEADER_FILES pose.h)


## --------------------------- Config ----------------------------
## Define the common library
add_library(${LIB_NAME} ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

## Link External Libraries
target_link_libraries(${LIB_NAME} PRIVATE Threads::Threads Eigen3::Eigen)

## Specify the root from which headers are defined
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)