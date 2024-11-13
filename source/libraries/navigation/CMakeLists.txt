set(LIB_NAME rca_navigation)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Define Sources
# list(APPEND SOURCE_FILES robot.cpp)

## Define Headers
list(APPEND HEADER_FILES trajectory_follower.h)
list(APPEND HEADER_FILES node_trajectory.h)


## --------------------------- Config ----------------------------
## Define the common library
add_library(${LIB_NAME} ${HEADER_FILES} ${SOURCE_FILES})

## Link External Libraries
target_link_libraries(${LIB_NAME} PRIVATE rca_common)

## Specify the root from which headers are defined
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
