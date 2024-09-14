/**
 * @file system.h
 * @author Kevin Orbie
 * 
 * @brief Declares the remote robot class (simulates a direct connection to the robot).
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <memory>

/* Custom C++ Libraries */
#include "common/looper.h"


namespace remote {
/* ========================== Classes ========================== */
class Robot: public Looper {
   public:
    Robot();

    void iteration() override;

   private:
    // client::Client client;
};


} // namespace remote