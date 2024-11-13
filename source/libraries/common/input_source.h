/**
 * @brief Input Source interface.
 */

#pragma once

/* ========================== Include ========================== */
/* C/C++ Libraries */
#include <vector>

/* Third Party Libraries */
// None

/* Custom C++ Libraries */
#include "common/input.h"


/* ========================== Classes ========================== */
/**
 * @brief The interface a class should implement if it produces user Input.
 */
class InputSource {
   public:
    /**
     * @note This virtual destructor is needed to allow derived classes to be polymophically destructed.
     * @link https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
     */
    virtual ~InputSource(){}; 

    /* Rule of Five. */
    InputSource()                                    = default;
    InputSource(InputSource && other)                  = default;
    InputSource(const InputSource& other)              = default;
    InputSource& operator=(InputSource && other)       = default;
    InputSource& operator=(const InputSource& other)   = default;

    virtual Input getInput() = 0;
};

