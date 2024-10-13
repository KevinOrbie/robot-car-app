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
class Robot final: public client::Client, public InputSink, public FrameProvider {
   public:
    Robot(std::string server_address, int port): client::Client(server_address, port){};
    void connect() override;

    /* Looper. */
    void iteration() override;
    void thread() override;
    void stop() override;

    /* Input Sink. */
    void sink(Input input) override;

    /* Frame Provider. */
    Frame getFrame(double curr_time, PixelFormat fmt);
    void startStream();
    void stopStream();

   private:
    std::unique_ptr<MessageHandler> message_handler_;
};


} // namespace remote