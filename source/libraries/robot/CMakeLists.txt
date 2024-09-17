set(LIB_NAME rca_video)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Define Sources
list(APPEND SOURCE_FILES system.cpp)
list(APPEND SOURCE_FILES arduino_socket.cpp)
list(APPEND SOURCE_FILES arduino_driver.cpp)

## Define Headers
list(APPEND HEADER_FILES system.h)
list(APPEND HEADER_FILES arduino_socket.h)
list(APPEND HEADER_FILES arduino_driver.h)


## --------------------------- Config ----------------------------
## Define the common library
add_library(rca_robot ${HEADER_FILES} ${SOURCE_FILES})

## Specify the root from which headers are defined
target_include_directories(rca_robot PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
