/**
 * @file timer.h
 * @author Kevin Orbie
 * 
 * @brief Defines a timer class with a stopwatch functionality.
 */

#ifndef TIMER_H
#define TIMER_H

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <chrono>

/* Custom C++ Libraries */
// None


/* =========================== Class =========================== */
class Timer final {
   public:
    /**
     * @brief Set a start time.
     */
    void start(){
        start_time_ = std::chrono::high_resolution_clock::now();
    };

    /**
     * @brief Get time in seconds (as double), since last call to start.
     */
    double stop(){
        std::chrono::time_point<std::chrono::high_resolution_clock> stop_time = std::chrono::high_resolution_clock::now();
        std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time_);
        return std::chrono::duration<double>(duration).count();
    };

    /**
     * @brief Get time in seconds (as double), since previous lap call or, if not available, since start time.
     */
    double lap(){
        /* First execute time sensitive code. */ 
        std::chrono::time_point<std::chrono::high_resolution_clock> lap_time = std::chrono::high_resolution_clock::now();
        
        if (start_time_ > prev_lap_time_)
        { /* No lap called yet. */
            prev_lap_time_ = lap_time;
            return stop();
        } else 
        { /* Lap already called. */
            std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(lap_time - prev_lap_time_);
            prev_lap_time_ = lap_time;
            return std::chrono::duration<double>(duration).count();
        }
    };

   private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
    std::chrono::time_point<std::chrono::high_resolution_clock> prev_lap_time_;
};

#endif
