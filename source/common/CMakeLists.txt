set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Define Sources
list(APPEND SOURCE_FILES client.cpp)
list(APPEND SOURCE_FILES server.cpp)
list(APPEND SOURCE_FILES connection.cpp)

## Define Headers
list(APPEND HEADER_FILES connection.h)
list(APPEND HEADER_FILES messages.h)
list(APPEND HEADER_FILES client.h)
list(APPEND HEADER_FILES server.h)
list(APPEND HEADER_FILES logger.h)
list(APPEND HEADER_FILES robot.h)

## Define the common library
add_library(rca_common ${HEADER_FILES} ${SOURCE_FILES})

## Specify the root from which headers are defined
target_include_directories(rca_common PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)