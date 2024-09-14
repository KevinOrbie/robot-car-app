set(LIB_NAME rca_common)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Define Sources
list(APPEND SOURCE_FILES client.cpp)
list(APPEND SOURCE_FILES server.cpp)
list(APPEND SOURCE_FILES connection.cpp)
list(APPEND SOURCE_FILES message_transciever.cpp)

list(APPEND SOURCE_FILES client_handler.cpp)
list(APPEND SOURCE_FILES server_handler.cpp)

## Define Headers
list(APPEND HEADER_FILES message_transciever.h)
list(APPEND HEADER_FILES drive_controller.h)
list(APPEND HEADER_FILES connection.h)
list(APPEND HEADER_FILES data_type.h)
list(APPEND HEADER_FILES messages.h)
list(APPEND HEADER_FILES message.h)
list(APPEND HEADER_FILES looper.h)
list(APPEND HEADER_FILES client.h)
list(APPEND HEADER_FILES server.h)
list(APPEND HEADER_FILES logger.h)
list(APPEND HEADER_FILES robot.h)


## --------------------------- Config ----------------------------
## Define the common library
add_library(${LIB_NAME} ${HEADER_FILES} ${SOURCE_FILES})

## Specify the root from which headers are defined
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)