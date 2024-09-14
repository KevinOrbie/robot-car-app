/**
 * @file system.h
 * @author Kevin Orbie
 * 
 * @brief Declares the main remote system-class, which indirectly holds all runtime data.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <memory>

/* Custom C++ Libraries */
#include "robot.h"


namespace remote {
/* ========================== Classes ========================== */

/**
 * @brief Represents a complete back-end System
 * 
 * @note Later on this class can be used to interconnect multiple different systems 
 * (e.g. controller, pipeline, etc.).
 */
class System {
   public:
    System();

    /**
     * @brief Run all loopers in a sperate thread.
     */
    void runMultiThreaded();

    /**
     * @brief Run all loopers in the current thread. Iterativley running through 
     * a single iteration of all Looper objects, ad infinitum.
     */
    void runSingleThreaded();

   private:
    std::unique_ptr<Robot> robot_;
    // std::unique_ptr<int> controller;
};


} // namespace remote