/**
 * @file control_panel.h
 * @author Kevin Orbie
 * 
 * @brief Declares an interface to the GUI System.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <memory>

/* Custom C++ Libraries */
#include "video/frame_provider.h"
#include "common/input_sink.h"
#include "common/looper.h"

/* Custom C++ Project Code */
#include "platform/window.h"
#include "common/timer.h"
#include "logic/logic.h"


/* ========================== Classes ========================== */
class ControlPanel: public Looper {
   public:
    ControlPanel(FrameProvider *frame_provider=nullptr, InputSink *input_sink=nullptr);

    void iteration() override;

    void setup() override;
    void cleanup() override;

   private:
    Timer stopwatch_;
    Window *window_ = nullptr;
    std::unique_ptr<Application> application_ = nullptr;
};
