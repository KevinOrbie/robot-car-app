/**
 * @file looper.h
 * @author Kevin Orbie
 * 
 * @brief Declares the looper interface.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <thread>
#include <mutex>

/* Custom C++ Libraries */
#include "logger.h"


/* ========================== Classes ========================== */
class Looper {
   public:
    virtual ~Looper() = default;  /* Runtime Polymorphism. */

    /* Rule of Five. */
    Looper()                                 = default;
    Looper(Looper && other)                  = default;
    Looper(const Looper& other)              = default;
    Looper& operator=(Looper && other)       = default;
    Looper& operator=(const Looper& other)   = default;

    /**
     * @brief Runs one loop iteration.
     */
    virtual void iteration() = 0;

    /**
     * @brief Starts the infinite loop.
     */
    virtual void start() {
        { /* Keep lock scope to a minimum. */
            std::lock_guard<std::mutex> lock(running_mutex_);
            running_ = true;
        }
        loopInfinitely();
    }

    /**
     * @brief Breaks out of the infinite loop.
     */
    virtual void stop() {
        {
            std::lock_guard<std::mutex> lock(running_mutex_);
            running_ = false;
        }

        if (thread_.joinable()){ /* Only valid if running in seperate thread. */
            thread_.join();
        }

        threaded_ = false;
    };

    /**
     * @brief Starts an infinite loop in different thread.
     */
    void thread() {
        threaded_ = true;
        thread_ =  std::thread(&Looper::start, this);
    };

   protected:
    /**
     * @brief Setup the thread where the infinite loop will run.
     * @note Does nothing by default.
     */
    virtual void setup() { return; }

    /**
     * @brief Cleanup the thread where the infinite loop ran.
     * @note Does nothing by default.
     */
    virtual void cleanup() { return; }

    /**
     * @brief Keep calling iteration(), until stop() is called.
     */
    void loopInfinitely() {
        setup();

        /* Start infinite loop. */
        while (true) {
            { /* Break out of loop if stopped. */
                // NOTE: Possibly use a conditional mutex?
                std::lock_guard<std::mutex> lock(running_mutex_);
                if (!running_) { break; }
            }
            iteration();
        }

        cleanup();
    }

    /**
     * @brief Return if this looper is running in a seperate thread.
     */
    bool threaded() {
        return threaded_;
    }

   protected:
    std::thread thread_;
    bool threaded_ = false;

    bool running_ = false;
    std::mutex running_mutex_;
};
