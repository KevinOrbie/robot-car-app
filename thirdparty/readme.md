# Third Party Libraries
Below, we describe how to obtain the required libraries for this project.

## Using Docker
If you have docker installed, you can build and run the code in a Docker container built using the provided `Dockerfile`:
```shell
# Build the docker file
docker build -f </path/to/Dockerfile> -t rca:v1 </path/to/project/root/>

# Run the RCA container (in the background)
docker run -it --rm \
    --detach \
	--gpus all \
    --privileged \
	--network=host \
	--name rca_env \
	--workdir /home/user/RCA \
	--runtime=nvidia \
    -v /.vscode/vscode-server:$HOME/.vscode-server \
	-v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY -e XAUTHORITY -e NVIDIA_DRIVER_CAPABILITIES=all \
	-h rca -v </path/to/project/root/>:/home/user/RCA \
	rca:v1

# Connect to the running container
docker exec -it -e 'TERM=xterm-256color' rca_env bash
```

**Alternativley**, you can use the aliases provided by `aliases_host.sh`:
```shell
rca-docker-build    # Build the RCA container
rca-docker-run      # Start an RCA container in the background
rca-docker-stop     # Stop the running RCA container
rca-docker-exec     # Connect to the running RCA container in the current shell
```

<span style="color:gold">WARNING</span>: Due to legacy reasons, not all the libraries required to run the GUI are included in the Docker Image. If you want to run the GUI Application, you will need to install the following libraries manually (see related sections below):
- [GLFW](#glfw-ubuntu-2404)
- [GLM](#glm)

## Manually Installing Libraries
If you want to run the code without docker, directly on your development device, you will need to install all the dependencies as described below.

### CMake (Ubuntu 18.04)
Cross-platform free and open-source software for build automation, testing, packaging and installation of software by using a compiler-independent method. ([source](https://en.wikipedia.org/wiki/CMake))
```shell
sudo apt update
sudo apt install cmake
```

### V4L2 (Ubuntu 18.04)
A collection of device drivers and an API for supporting realtime video capture on Linux systems. ([source](https://en.wikipedia.org/wiki/Video4Linux))

Download all V4L2 development files:
```shell
sudo apt-get update
sudo apt-get install libv4l-dev     # libv4l consists of 3 different libraries: libv4lconvert, libv4l1 and libv4l2.

# Optional
sudo apt-get install v4l-utils      # Included: v4l2-clt (An application to control video4linux drivers).
```

### FFmpeg (Ubuntu 18.04)
Free and open-source software project consisting of a suite of libraries and programs for handling video, audio, and other multimedia files and streams. ([source](https://en.wikipedia.org/wiki/FFmpeg))

Download all ffmpeg development files:
```shell
sudo apt update
sudo apt install libavcodec-dev libavdevice-dev libavfilter-dev libavformat-dev libavutil-dev libpostproc-dev libswresample-dev libswscale-dev
```

### GLFW (Ubuntu 24.04)
A lightweight utility library for use with OpenGL. It provides programmers with the ability to create and manage windows and OpenGL contexts, as well as handle joystick, keyboard and mouse input. ([source](https://en.wikipedia.org/wiki/GLFW))

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

### GLAD
Already **included** in version control.<br>
To keep custom and external code spereate, I created a CMake static library from the glad code.<br>
The glad code & headers were downloaded from the [GLAD website](http://glad.dav1d.de/), with the following settings:

![form setting to use](./glad/glad_form.JPG "GLAD download form")

### stb_image.h
Already **included** in version control.<br>
To keep custom and external code spereate, I created a CMake static library from the header file lib. <br>
This header only library was installed from [here](https://github.com/nothings/stb/blob/master/stb_image.h).

### GLM
OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications. GLM provides classes and functions designed and implemented with the same naming conventions and functionalities than GLSL so that anyone who knows GLSL, can use GLM as well in C++.

Already **parially included** in version control.<br>
The CMake library basis is already created, with a glm scope, but the glm source code still needs to be downloaded.

Setup this library by doing the following:
1. Download the latest light (header only) source code from [here](https://github.com/g-truc/glm/tags).
2. Extract the zip file, and put the project directly under the `thirdparty/glm/include/glm/` directory (the root CMakeLists.txt file should be direcly under the `./glm/include/glm/` directory).

