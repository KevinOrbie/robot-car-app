set(GTEST_LIBS "")

if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    list(APPEND GTEST_LIBS GTest::Main)
else()
    list(APPEND GTEST_LIBS GTest::gtest_main)
endif()


######## Create Google Test executable ########
add_executable(test_image test_image.cpp)

## Link Libraries
target_link_libraries(test_image ${GTEST_LIBS} rca_video)

## Keep test directory structure for the executable under the build directory
file(RELATIVE_PATH CURRENT_RELATIVE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(test_image PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CURRENT_RELATIVE_PATH}/)


######### Register tests with CTest #########
# This is similar to add_test()
gtest_discover_tests(test_image)
