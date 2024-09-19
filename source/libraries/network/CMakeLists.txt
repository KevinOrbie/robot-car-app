set(LIB_NAME rca_network)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Define Sources
list(APPEND SOURCE_FILES client.cpp)
list(APPEND SOURCE_FILES server.cpp)
list(APPEND SOURCE_FILES messages.cpp)
list(APPEND SOURCE_FILES connection.cpp)
list(APPEND SOURCE_FILES message_transciever.cpp)

## Define Headers
list(APPEND HEADER_FILES message_transciever.h)
list(APPEND HEADER_FILES message_handler.h)
list(APPEND HEADER_FILES connection.h)
list(APPEND HEADER_FILES messages.h)
list(APPEND HEADER_FILES message.h)
list(APPEND HEADER_FILES client.h)
list(APPEND HEADER_FILES server.h)


## --------------------------- Config ----------------------------
## Define the common library
add_library(${LIB_NAME} ${HEADER_FILES} ${SOURCE_FILES})

## Link External Libraries
target_link_libraries(${LIB_NAME} PRIVATE rca_common)

## Specify the root from which headers are defined
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)