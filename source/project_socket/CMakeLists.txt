set(TARGET_NAME socket)

## --------------------------- Sources ---------------------------
set(INSTALL_TARGETS "")
set(HEADER_FILES "")
set(SOURCE_FILES "")

## Include files in this directory
list(APPEND SOURCE_FILES client_class.cpp)	
list(APPEND SOURCE_FILES server_class.cpp)	

## Include code in sub-directories
# NONE

## Create Executable
add_executable(${TARGET_NAME}_client ${HEADER_FILES} client_class.cpp)
add_executable(${TARGET_NAME}_server ${HEADER_FILES} server_class.cpp)
list(APPEND INSTALL_TARGETS ${TARGET_NAME}_client)	
list(APPEND INSTALL_TARGETS ${TARGET_NAME}_server)	

## Specify the root from which headers are defined
target_include_directories(${TARGET_NAME}_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TARGET_NAME}_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

## Link External Libraries
# NONE

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
