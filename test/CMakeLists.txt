
## Include Google Test Library
include(FetchContent)
cmake_policy(SET CMP0135 NEW)  # Disable Warning
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)  # Disable Warning
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)

## Setting GTest options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # For Windows: Prevent overriding the parent project's compiler/linker settings
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)  # Don't add GTest code to install directory (ON by default)
FetchContent_MakeAvailable(googletest)

## Add specific test scripts
add_subdirectory(unit)

## Make inputs available in '_build' directory
file(COPY ${CMAKE_SOURCE_DIR}/test/inputs DESTINATION ${CMAKE_BINARY_DIR}/test)
