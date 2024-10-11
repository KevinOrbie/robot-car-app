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
    /**
     * @note This virtual destructor is needed to allow derived classes to be polymophically destructed.
     * @link https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
     */
    virtual ~InputSink(){}; 
    virtual void sink(Input input) = 0;
};
