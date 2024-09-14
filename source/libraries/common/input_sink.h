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
// None


/* ========================== Classes ========================== */
class Input;

class InputSink {
   public:
    virtual void sink(Input input) = 0;
};
