set(LIB_NAME control_panel)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Include files in this directory
list(APPEND SOURCE_FILES control_panel.cpp)	
list(APPEND HEADER_FILES control_panel.h)	

## Include code in sub-directories
add_subdirectory(platform)
add_subdirectory(logic)


## --------------------------- Config ----------------------------
## Create Executable
add_library(${LIB_NAME} ${HEADER_FILES} ${SOURCE_FILES})

## Specify the root from which headers are defined
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

## Link External Libraries
target_link_libraries(${LIB_NAME} PUBLIC glad glfw stb_image glm rca_common)


## ------------------------- Installation -------------------------
## Set the install location if not specified by user (using -DCMAKE_INSTALL_PREFIX= when configuring a build tree).
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  ## By default, the deploy location is: c:/Program Files/${PROJECT_NAME}
  ## This changes it to the a project sub-directory (usefull for testing)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/_deploy CACHE PATH "deploy directory path" FORCE)
endif()

## Install the resources to the specified install directory.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_INSTALL_PREFIX}/controller)
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGET_NAME})
