/**
 * @brief Header for shared C++ Robot Funcitionality.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None


/* ========================== Classes ========================== */
namespace robot {

struct DriveController {
    enum class Direction {STRAIGHT, LEFT, RIGHT};
    enum class Throttle {STANDBY, FORWARD, REVERSE, BRAKE};

    struct State {
        Direction direction = Direction::STRAIGHT;
        Throttle throttle = Throttle::STANDBY;
        float speed = 1.0f;  // 1.0 is full speed, 0.0 is no speed

        bool operator!=(const State &other_state) {
            return (speed != other_state.speed) ||
                   (direction != other_state.direction) ||
                   (throttle != other_state.throttle);
        }
    };

    /* ---------------------- Variables --------------------- */
    State state = {};

    /* ------------------ Setter Functions ------------------ */
    void standby() { state.throttle = Throttle::STANDBY; };
    void brake()   { state.throttle = Throttle::BRAKE;   };
    void forward(float speed = 0.0f) { 
        state.throttle = Throttle::FORWARD; 
        if (speed > 0) {
            state.speed = speed; 
        }
    };
    void reverse(float speed = 0.0f) { 
        state.throttle = Throttle::REVERSE; 
        if (speed > 0) {
            state.speed = speed; 
        }
    };

    void straight() { state.direction = Direction::STRAIGHT; };
    void right()    { state.direction = Direction::RIGHT;    };
    void left()     { state.direction = Direction::LEFT;     };
};

} // namespace robot
