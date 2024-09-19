/**
 * @file robot.h
 * @author Kevin Orbie
 * 
 * @brief Declares the remote robot class (simulates a direct connection to the robot).
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "common/looper.h"
#include "common/input_sink.h"
#include "video/frame_provider.h"
#include "network/client.h"
#include "message_handler.h"


namespace remote {
/* ========================== Classes ========================== */

/**
 * @brief This is the interface to the robot for a remote controller.
 */
class Robot: public Looper, public InputSink, public FrameProvider {
   public:
    Robot(std::string server_address, int port): client_(server_address, port, false), handler_(client_){};

    void connect();

    /* Looper. */
    void iteration() override;

    /* Input Sink. */
    void sink(Input input) override;

    /* Frame Provider. */
    Frame getFrame(double curr_time) ;
    void startStream();
    void stopStream();

   private:
    client::Client client_;
    MessageHandler handler_;
};


} // namespace remote