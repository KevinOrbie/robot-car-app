/**
 * @file logic.h
 * @author Kevin Orbie
 * 
 * @brief Defines all game logic.
 */

/* ========================== Include ========================== */
/* C/C++ Libraries */
// None

/* Third Party Libraries */
// None

/* Custom C++ Libraries */
#include "platform/input.h"


namespace game {
/* ========================= Functions ========================= */

int initialize();
int processFrame(float timedelta, int width, int height, Input& input);
int destruct();

} // namespace game
