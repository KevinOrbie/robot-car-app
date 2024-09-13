/**
 * @brief Header for our C++ Messaging functionality.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "connection.h"
#include "logger.h"


/* ========================== Classes ========================== */
struct Serializable {
    virtual void serialize(Connection &connection) = 0;
    virtual void deserialize(Connection &connection) = 0;
};

struct DriveControl: public Serializable {
    enum class Direction {STRAIGHT, LEFT, RIGHT};
    enum class Throttle {STANDBY, FORWARD, REVERSE, BRAKE};

    float speed = 0.0f;
    Throttle throttle = Throttle::STANDBY;
    Direction direction = Direction::STRAIGHT;

    void serialize(Connection &connection) override {
        connection.send(reinterpret_cast<char*>(&speed)     , sizeof(speed));
        connection.send(reinterpret_cast<char*>(&direction) , sizeof(direction));
        connection.send(reinterpret_cast<char*>(&throttle)  , sizeof(throttle));
    };

    void deserialize(Connection &connection) override {
        connection.recieve(reinterpret_cast<char*>(&speed)     , sizeof(speed));
        connection.recieve(reinterpret_cast<char*>(&direction) , sizeof(direction));
        connection.recieve(reinterpret_cast<char*>(&throttle)  , sizeof(throttle));
    };
};


