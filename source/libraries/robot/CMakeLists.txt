set(LIB_NAME rca_video)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Define Sources
list(APPEND SOURCE_FILES system.cpp)

## Define Headers
list(APPEND HEADER_FILES system.h)


## --------------------------- Config ----------------------------
## Define the common library
add_library(rca_robot ${HEADER_FILES} ${SOURCE_FILES})

## Specify the root from which headers are defined
target_include_directories(rca_robot PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
