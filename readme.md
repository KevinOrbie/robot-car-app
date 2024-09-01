# Robotic Car Application
An application to support a custom robotic car.<br>
It containts both the program to run on the car target and an OpenGL based GUI application to run on a desktop.

## Setup
1. Clone this repository: 
```sh
git clone https://github.com/KevinOrbie/robot-car-app ./RCA
```

2. Download and setup the required thirparty packages, as described [here](./thirdparty/readme.md).

3. (optional) Source the `aliases.sh` file your `.bashrc`: adds a bunch of ease of use development aliases starting with "`rca-`".

## Build Instructions (Ubuntu)

### Using Aliases
```shell
rca-setup-build-release  # Setup Build System
rca-build-install        # Build & Install App
rca-run                  # Run the Application
```

### Using Vanilla Commands
```shell
# Run all commands below from in the project root directory
cd <path/to/opengl-workspace>

# Setup build system (locally)
cmake -B./_build -DCMAKE_BUILD_TYPE=Release

# Compile Program & Install
# NOTE: the --prefix option is not supported, see CMakeLists.txt for more info.
cmake --build ./_build && cmake --install ./_build

# Run the Application
(cd ./_deploy/grass && ./grass)
```


## Run Insturctions
1. Start the desired executable.
