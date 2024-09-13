/**
 * @brief Header for remote C++ Robot Funcitionality.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom Libraries */
#include "common/robot.h"
#include "common/client.h"
#include "platform/input.h"


/* ========================== Classes ========================== */
namespace robot { 
namespace remote {
    
struct DriveController: public robot::DriveController {
    State last_state = {};

    void process(Input &input) {
        /* Process Direction. */
        if (input.keys[Button::LEFT].held == input.keys[Button::RIGHT].held) {
            straight();
        } else if (input.keys[Button::LEFT].held) {
            left();
        } else if (input.keys[Button::RIGHT].held) {
            right();
        }

        /* Process Throttle. */
        if (input.keys[Button::UP].held && input.keys[Button::DOWN].held) {
            brake();
        } else if (input.keys[Button::UP].held) {
            forward();
        } else if (input.keys[Button::DOWN].held) {
            reverse();
        } else {
            standby();
        }
    };

    void send(Client &client) {
        /* Check if there was a state update. */
        if (last_state != state) {
            // TODO: send message over 
            //client.send();
            last_state = state;
        }
        
        return;
    };
};

} // namespace remote 
} // namespace robot