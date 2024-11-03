/**
 * @brief Input Sink interface.
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
 * @brief The interface a class should implement if it accepts user Input.
 */
class InputSink {
   public:
    /**
     * @note This virtual destructor is needed to allow derived classes to be polymophically destructed.
     * @link https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
     */
    virtual ~InputSink(){}; 

    /* Rule of Five. */
    InputSink()                                    = default;
    InputSink(InputSink && other)                  = default;
    InputSink(const InputSink& other)              = default;
    InputSink& operator=(InputSink && other)       = default;
    InputSink& operator=(const InputSink& other)   = default;

    virtual void sink(Input input) = 0;
};


class InputSinkSplitter: public InputSink {
   public:
    InputSinkSplitter(std::vector<InputSink*> sinks = {}): sinks_(sinks) {};
    void sink(Input input) {
        for (InputSink *sink: sinks_) {
            sink->sink(input);
        }
    };

    void add(InputSink* sink) {
        sinks_.push_back(sink);
    }

   private:
    std::vector<InputSink*> sinks_ = {};
};
