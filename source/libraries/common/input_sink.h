/**
 * @brief Input Sink interface.
 */

#pragma once

/* ========================== Include ========================== */
/* C/C++ Libraries */
// None

/* Third Party Libraries */
// None

/* Custom C++ Libraries */
#include "common/input.h"


/* ========================== Classes ========================== */
/**
 * @brief The interface a class should implement if it accepts user Input.
 */
class InputSink {
   public:
    virtual void sink(Input input) = 0;
};