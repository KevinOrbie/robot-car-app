## Update local values
list(APPEND HEADER_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/remote.h 
)


## Propagate values to parent's scope
set(HEADER_FILES ${HEADER_FILES} PARENT_SCOPE)