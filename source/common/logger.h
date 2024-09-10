/**
 * @file logger.h
 * @author Kevin Orbie
 * 
 * @brief Defines macros to setup the project's logging inferastructure.
 * @note By exclusivley making use of macros, we make it possible to compile without any print statements.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdio.h>


/* ========================== Macros =========================== */
#define COLORED  // Uncomment to log wihtout color

#define _LOG(lvl, message, ...) fprintf(stderr, "[" lvl "] " message " (%s: %d)\n", ## __VA_ARGS__,  __func__, __LINE__)
#define _LOG_COLOR(lvl, hcolor, mcolor, message, ...) fprintf(stderr, mcolor "[" hcolor lvl mcolor "] " message "  \e[0;38;2;140;140;140m (%s: %d) \e[0m\n", ## __VA_ARGS__,  __func__, __LINE__)
#define _LOG_COMPLEX(lvl, message, ...) fprintf(stderr, "[" lvl "] " message " (%s - %s: %d)\n", ## __VA_ARGS__, __FILE__, __func__, __LINE__)

#ifndef COLORED
    #define LOGI(message, ...) _LOG(" INFO  ", message, ## __VA_ARGS__)
    #define LOGW(message, ...) _LOG(" WARN  ", message, ## __VA_ARGS__)
    #define LOGE(message, ...) _LOG(" ERR  ", message, ## __VA_ARGS__)
#else
    #define LOGI(message, ...) _LOG_COLOR(" INFO ", "\e[1m", "\e[0m", message, ## __VA_ARGS__)
    #define LOGW(message, ...) _LOG_COLOR(" WARN ", "\e[1;38;2;245;197;24m", "\e[0;38;2;220;184;48m", message, ## __VA_ARGS__)
    #define LOGE(message, ...) _LOG_COLOR(" ERR  ", "\e[1;38;2;245;43;24m", "\e[0;38;2;220;79;89m", message, ## __VA_ARGS__)
#endif
