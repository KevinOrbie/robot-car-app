/**
 * @file logger.cpp
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */
#include "logger.h"

/* Standard C Libraries */
#include <stdio.h>
#include <stdarg.h>

/* Standard C++ Libraries */
#include <mutex>


/* ========================= Functions ========================= */
void _logger_printf(const char* format, ...) {
    static std::mutex log_mutex;

    va_list argptr;
    va_start(argptr, format);

    { /* Thread-safe access to stderr. */
        std::lock_guard lock(log_mutex);
        vfprintf(stderr, format, argptr);
    }

    va_end(argptr);
};
