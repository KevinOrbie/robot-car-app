/**
 * @file remote.cpp
 * @author Kevin Orbie
 * 
 * @brief Defines the remote gui class (simulates a direct connection to the remote).
 */

/* ========================== Include ========================== */
#include "remote.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "common/logger.h"


namespace robot {
/* ========================== Classes ========================== */
void Remote::connect() {
    server_.connect();
}

void Remote::iteration() {
    server_.iteration();
    handler_.iteration();
};


} // namespace robot