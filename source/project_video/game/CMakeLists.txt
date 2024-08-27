## Update local values
list(APPEND HEADER_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/quad_screen.h 
  ${CMAKE_CURRENT_SOURCE_DIR}/frame_provider.h 
  ${CMAKE_CURRENT_SOURCE_DIR}/logic.h
)

list(APPEND SOURCE_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/logic.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/frame_provider.cpp 
)


## Propagate values to parent's scope
set(HEADER_FILES ${HEADER_FILES} PARENT_SCOPE)
set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)