# Third Party Libraries
Below, we describe how to obtain the required libraries for this project.

## CMake 
```shell
sudo apt update
sudo apt install cmake
```

## FFmpeg (Ubuntu 24.04)
Download all ffmpeg development files:
```shell
sudo apt update
sudo apt install libavcodec-dev libavdevice-dev libavfilter-dev libavformat-dev libavutil-dev libpostproc-dev libswresample-dev libswscale-dev
``` 

## GLFW (Ubuntu 24.04)
Specific compilation documentation can be found [here](https://www.glfw.org/docs/latest/compile.html).
From the thirdparty directory, run:
```shell
# Download Source Code
git clone https://github.com/glfw/glfw.git ./glfw
cd glfw

# Install dependencies
sudo apt install libwayland-dev libxkbcommon-dev xorg-dev

# Generate CMake Build System
cmake -B./_build

# Build & Install GLFW
cmake --build ./_build; cmake --install ./_build --prefix ./_deploy
```

## GLAD
Already **included** in version control.<br>
To keep custom and external code spereate, I created a CMake static library from the glad code.<br>
The glad code & headers were downloaded from the [GLAD website](http://glad.dav1d.de/), with the following settings:

![form setting to use](./glad/glad_form.JPG "GLAD download form")

## stb_image.h
Already **included** in version control.<br>
To keep custom and external code spereate, I created a CMake static library from the header file lib. <br>
This header only library was installed from [here](https://github.com/nothings/stb/blob/master/stb_image.h).

## GLM
OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications. GLM provides classes and functions designed and implemented with the same naming conventions and functionalities than GLSL so that anyone who knows GLSL, can use GLM as well in C++.

Already **parially included** in version control.<br>
The CMake library basis is already created, with a glm scope, but the glm source code still needs to be downloaded.

Setup this library by doing the following:
1. Download the latest light (header only) source code from [here](https://github.com/g-truc/glm/tags).
2. Extract the zip file, and put the project directly under the `thirdparty/glm/include/glm/` directory (the root CMakeLists.txt file should be direcly under the `./glm/include/glm/` directory).

