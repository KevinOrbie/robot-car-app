## Create Google Test executable
add_executable(csv_test csv_test.cpp)

## Link Libraries
target_link_libraries(csv_test GTest::gtest_main utils)

## Include Library Headers
target_include_directories(csv_test PRIVATE ${CMAKE_SOURCE_DIR}/source/utils)

## Keep test directory structure for the executable under the build directory
file(RELATIVE_PATH CURRENT_RELATIVE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})  # CURRENT_RELATIVE_PATH = test/unit/utils
set_target_properties(csv_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CURRENT_RELATIVE_PATH}/)

## Register test with CTest
gtest_discover_tests(csv_test)
