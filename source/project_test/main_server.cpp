/**
 * @brief Main application running on the robot.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */

/* Standard C++ Libraries */
#include <thread>
#include <chrono>

/* Third Party Libraries */
// None

/* Custom Includes */
#include "common/message.h"
#include "common/server.h"


/* ======================== Entry Point ======================== */
int main() {
    /* Setup LAN connection. */
    Server server = Server(2556, true);

    /* Simulate control loop. */
    while (true) {
        /* Recieve commands over LAN. */
        std::unique_ptr<MessageData> data = server.recieve();
        
        if (data) {
            TextMessage *msg = dynamic_cast<TextMessage*>(data.get());
            fprintf(stderr, "Recieved message: '%s'\n", msg->text.c_str());
        }

        /* Slow down loop. */
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}


// TODO: Make really clear what I want:
// > Easy to send messages
// > Clear structure
// > Easy to add a new message
// really figure this out ...

// > Do I need message class?
//    - Maybe just make the Send Message automatically add a header (only multi-part UDP messages might need extra class).
// > Do I need to make serialize more general, to help in testing (not linked to connection).
// > How do I want the user to define new messages.
//    - Maybe I just want an object be able to be serializable, and then I can create a message for it, but don't tigh the messaging directly to the object?
//    - Maybe add Serializable class: with a function that expects another abstract class with a read and write function as input 
//    - And create serialize functions for all basic values (int, float, double, etc., i.e. serialize(dst, float))
// > What about the response
// > Is this ready for operation in a seperate thread.
//    - Blocking wait for messages, and message handler.
// > What about UDP?
// > ...
