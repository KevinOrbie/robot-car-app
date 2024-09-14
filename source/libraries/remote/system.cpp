/**
 * @file system.cpp
 * @author Kevin Orbie
 * 
 * @brief Defines the main remote system-class, which indirectly holds all runtime data.
 */

/* ========================== Include ========================== */
#include "system.h"

/* Standard C Libraries */
// None

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "common/logger.h"


namespace remote {
/* ========================== Classes ========================== */
System::System() {

};

void System::runMultiThreaded() {
    robot_->thread();
};

void System::runSingleThreaded() {
    while (true) {
        robot_->iteration();
    }
};


} // namespace remote