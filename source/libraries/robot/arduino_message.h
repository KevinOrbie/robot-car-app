/**
 * @file arduino_message.h
 * @author Kevin Orbie
 * 
 * @brief Declares the arduino message structure (with STL).
 */

#pragma once


/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
#include <vector>

/* Custom C++ Libraries */
#include "arduino_types.h"


namespace arduino {

/* ========================= Messaging ========================= */
struct Message {
    MessageID id = MessageID::EMPTY;
    std::vector<uint8_t> data = {};
};

} // namespace arduino
