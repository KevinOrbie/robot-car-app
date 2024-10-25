/**
 * @file arduino_message.h
 * @author Kevin Orbie
 * 
 * @brief Declares the arduino message structure (without STL & not thread-safe).
 * @warning To be used only for single threaded operation, with limited memory overhead.
 */

#pragma once


/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "./public/arduino_types.h"


namespace arduino {

/* ========================= Messaging ========================= */
struct Message {
    MessageID id = MessageID::EMPTY;
    uint8_t *data = nullptr;
    int num_data_bytes = 0;
};

} // namespace arduino
