set(GTEST_LIBS "")

if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    list(APPEND GTEST_LIBS GTest::Main)
else()
    list(APPEND GTEST_LIBS GTest::gtest_main)
endif()


######## Create Google Test executable ########
add_executable(test_logging test_logging.cpp)
add_executable(test_pose    test_pose.cpp)

## Link Libraries
target_link_libraries(test_logging ${GTEST_LIBS} rca_common)
target_link_libraries(test_pose    ${GTEST_LIBS} rca_common)

## Include Library Headers
# target_include_directories(test_logging PRIVATE ${CMAKE_SOURCE_DIR}/source/utils)
# target_include_directories(test_pose    PRIVATE ${CMAKE_SOURCE_DIR}/source/utils)

## Keep test directory structure for the executable under the build directory
file(RELATIVE_PATH CURRENT_RELATIVE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})  # CURRENT_RELATIVE_PATH = test/unit/utils
set_target_properties(test_logging PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CURRENT_RELATIVE_PATH}/)
set_target_properties(test_pose    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CURRENT_RELATIVE_PATH}/)


######### Register tests with CTest #########
# This is similar to add_test()
gtest_discover_tests(test_logging)
gtest_discover_tests(test_pose)
