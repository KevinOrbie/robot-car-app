## Include Libraries
add_subdirectory(libraries/common)
add_subdirectory(libraries/network)
add_subdirectory(libraries/remote)
add_subdirectory(libraries/robot)
add_subdirectory(libraries/video)
add_subdirectory(libraries/navigation)

if((NOT DEFINED ENV{HOST}) OR (NOT $ENV{HOST} STREQUAL "robot"))
    # Requires extra libraries
    add_subdirectory(libraries/control_panel)
endif()

## Include Projects
add_subdirectory(projects/engine)

# Don't build the controller on the robot.
if((NOT DEFINED ENV{HOST}) OR (NOT $ENV{HOST} STREQUAL "robot"))
    # Requires extra libraries
    add_subdirectory(projects/controller)
    add_subdirectory(projects/arduino_cli)
endif()