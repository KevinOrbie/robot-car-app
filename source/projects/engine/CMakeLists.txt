set(TARGET_NAME engine)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Include files in this directory
list(APPEND SOURCE_FILES main.cpp)	
list(APPEND SOURCE_FILES arduino_socket.cpp)	

list(APPEND HEADER_FILES arduino_socket.h)	
list(APPEND HEADER_FILES local.h)	

## Include code in sub-directories
# None

## Create Executable
add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})
list(APPEND INSTALL_TARGETS ${TARGET_NAME})	

## Specify the root from which headers are defined
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

## Link External Libraries
target_link_libraries(${TARGET_NAME} PRIVATE rca_common rca_robot)


## ------------------------- Installation -------------------------
## Set the install location if not specified by user (using -DCMAKE_INSTALL_PREFIX= when configuring a build tree).
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  ## By default, the deploy location is: c:/Program Files/${PROJECT_NAME}
  ## This changes it to the a project sub-directory (usefull for testing)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/_deploy CACHE PATH "deploy directory path" FORCE)
endif()

## Install to the specified install directory.
install(TARGETS ${INSTALL_TARGETS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGET_NAME})
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGET_NAME})
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGET_NAME})
