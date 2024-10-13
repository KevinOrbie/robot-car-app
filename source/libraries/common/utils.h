/**
 * @file utils.h
 * @author Kevin Orbie
 * 
 * @brief A collection of random utilities that are too small to fit elsewhere.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <unistd.h>         // gettid()
#include <sys/syscall.h>    // syscall()

/* Standard C++ Libraries */
// None


/* ========================== Macros =========================== */
/**
 * @brief Get the thread ID of the current thread.
 * @note Work around glibc bug (glibc version < 2.30).
 * @link https://stackoverflow.com/questions/30680550/c-gettid-was-not-declared-in-this-scope
 */
#define gettid() syscall(SYS_gettid)
