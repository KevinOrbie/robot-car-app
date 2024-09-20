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

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdio.h>

/* Standard C++ Libraries */
// None


/* ========================== Macros =========================== */
#define COLORED  // Uncomment to log wihtout color

/* Color Definitions */
#ifdef COLORED
    #define _LOG_COLOR_BOLD         "\e[1m"
    #define _LOG_COLOR_BOLD_YELLOW  "\e[1;38;2;245;197;24m"
    #define _LOG_COLOR_YELLOW       "\e[0;38;2;220;184;48m"
    #define _LOG_COLOR_BOLD_RED     "\e[1;38;2;245;43;24m"
    #define _LOG_COLOR_RED          "\e[0;38;2;220;79;89m"
    #define _LOG_COLOR_GREY         "\e[0;38;2;140;140;140m"
    #define _LOG_COLOR_CLEAR        "\e[0m"
#else
    #define _LOG_COLOR_BOLD         ""
    #define _LOG_COLOR_BOLD_YELLOW  ""
    #define _LOG_COLOR_YELLOW       ""
    #define _LOG_COLOR_BOLD_RED     ""
    #define _LOG_COLOR_RED          ""
    #define _LOG_COLOR_GREY         ""
    #define _LOG_COLOR_CLEAR        ""
#endif

/* Create Message with arguments */
#define _LOG_ARGS_SHRT(lvl, hcolor, mcolor, message, ...) mcolor "[" hcolor lvl mcolor "] " message _LOG_COLOR_GREY "  (%s: %d)" _LOG_COLOR_CLEAR "\n", ## __VA_ARGS__,  __func__, __LINE__
#define _LOG_ARGS_LONG(lvl, hcolor, mcolor, message, ...) mcolor "[" hcolor lvl mcolor "] " message _LOG_COLOR_GREY "  ( %s:%d , %s() )" _LOG_COLOR_CLEAR "\n", ## __VA_ARGS__, __FILE__, __LINE__, __func__

/* Logging Definitions */
#define LOGI(message, ...) _logger_printf(_LOG_ARGS_SHRT(" INFO ", _LOG_COLOR_BOLD       , _LOG_COLOR_CLEAR , message, ## __VA_ARGS__))
#define LOGW(message, ...) _logger_printf(_LOG_ARGS_LONG(" WARN ", _LOG_COLOR_BOLD_YELLOW, _LOG_COLOR_YELLOW, message, ## __VA_ARGS__))
#define LOGE(message, ...) _logger_printf(_LOG_ARGS_LONG(" ERR  ", _LOG_COLOR_BOLD_RED   , _LOG_COLOR_RED   , message, ## __VA_ARGS__))


/* ========================= Functions ========================= */
void _logger_printf(const char* format, ...);

