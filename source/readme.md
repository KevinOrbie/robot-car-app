# Source Code
This directory contains all custom code for this project.

## File Structure

### Libraries
- **common**: Code used in multiple projects / other libs.
- **network**: Network interface and functionality.
- **video**: Video Functionality.
- **remote**: (PROJECT BASE) The code that runs remotly.
- **robot**: (PROJECT BASE) The code that runs on the robot.

### Projects
- **controller**: The remote controller for the robot.
- **engine**: The project that should run on the robot.


## Locations

### Messages
- [messages.h](./libraries/network/messages.h): Defines all the messages.
- [messages.cpp](./libraries/network/messages.cpp): Add a mapping to the correct deserializer for all messages.
- [message_handler.h](./libraries/network/message_handler.h): Adds a default handler to the handler interface.
- [remote::MessageHandler](./libraries/remote/message_handler.h): Handles a message recieved by the controller.
- [client::MessageHandler](./libraries/robot/message_handler.h): Handles handle a message recieved by the robot.
