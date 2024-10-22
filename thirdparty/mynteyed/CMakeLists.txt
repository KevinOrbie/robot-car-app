cmake_minimum_required(VERSION 3.9)

# Brief: Run all setup required to run MYNT-EYE-D-SDK with CMAKE
# Link: https://mynt-eye-d-sdk.readthedocs.io/en/latest/sdk/project_cmake.html


## ---------------------- MYNT-EYE-D-SDK ----------------------
find_package(mynteyed REQUIRED)
message(STATUS "Found mynteye: ${mynteyed_VERSION}")

# When SDK build with OpenCV, we can add WITH_OPENCV macro to enable some
# features depending on OpenCV, such as ToMat().
if(mynteyed_WITH_OPENCV)
  add_definitions(-DWITH_OPENCV)
endif()


## -------------------------- OpenCV -------------------------
# When SDK build with OpenCV, we must find the same version here.
find_package(OpenCV REQUIRED)
message(STATUS "Found OpenCV: ${OpenCV_VERSION}")
