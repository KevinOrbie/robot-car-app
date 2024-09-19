/**
 * @file logger.h
 * @author Kevin Orbie
 * 
 * @brief Defines macros to setup the project's logging inferastructure.
 * @note By exclusivley making use of macros, we make it possible to compile 
 * without any print statements.
 * @note All logs should done in one line, as multiple printf statements for 
 * a single log can get messy when working with multiple threads.
 */

// TODO: Add the time to logs
// TODO: support multi thread logging

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdio.h>


/* ========================== Macros =========================== */
#define COLORED  // Uncomment to log wihtout color

#define _LOG(lvl, message, ...) fprintf(stderr, "[" lvl "] " message " (%s: %d)\n", ## __VA_ARGS__,  __func__, __LINE__)
#define _LOG_FULL(lvl, message, ...) fprintf(stderr, "[" lvl "] " message " ( %s:%d , %s() )\n", ## __VA_ARGS__,  __FILE__, __LINE__, __func__)
#define _LOG_COLOR(lvl, hcolor, mcolor, message, ...) fprintf(stderr, mcolor "[" hcolor lvl mcolor "] " message "  \e[0;38;2;140;140;140m (%s: %d) \e[0m\n", ## __VA_ARGS__,  __func__, __LINE__)
#define _LOG_COLOR_FULL(lvl, hcolor, mcolor, message, ...) fprintf(stderr, mcolor "[" hcolor lvl mcolor "] " message "  \e[0;38;2;140;140;140m ( %s:%d , %s() ) \e[0m\n", ## __VA_ARGS__, __FILE__, __LINE__, __func__)

#ifndef COLORED
    #define LOGI(message, ...) _LOG(" INFO  ", message, ## __VA_ARGS__)
    #define LOGW(message, ...) _LOG(" WARN  ", message, ## __VA_ARGS__)
    #define LOGE(message, ...) _LOG_FULL(" ERR  ", message, ## __VA_ARGS__)
#else
    #define LOGI(message, ...) _LOG_COLOR(" INFO ", "\e[1m", "\e[0m", message, ## __VA_ARGS__)
    #define LOGW(message, ...) _LOG_COLOR_FULL(" WARN ", "\e[1;38;2;245;197;24m", "\e[0;38;2;220;184;48m", message, ## __VA_ARGS__)
    #define LOGE(message, ...) _LOG_COLOR_FULL(" ERR  ", "\e[1;38;2;245;43;24m", "\e[0;38;2;220;79;89m", message, ## __VA_ARGS__)
#endif
