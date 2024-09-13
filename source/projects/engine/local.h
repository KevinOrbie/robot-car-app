/**
 * @brief Header for C++ Robot Funcitionality on the robot itself.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom Libraries */
#include "common/robot.h"
#include "arduino_socket.h"


/* ========================== Classes ========================== */
namespace robot { 
namespace local {

struct DriveController: public robot::DriveController {
    ArduinoSocket socket; // TODO: Initialize (possibly get from someone else, maybe from robot)

    void process(...) {};
    void update() {};  // Control Via Arduino
};

} // namespace local 
} // namespace robot