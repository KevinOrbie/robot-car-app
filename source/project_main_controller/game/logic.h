/**
 * @brief Defines all gamelogic.
 * @author Kevin Orbie
 */

/* ========================== Include ========================== */
/* C/C++ Libraries */
/* Third Party Libraries */
/* Custom Libraries */
#include "platform/input.h"


/* ========================= Functions ========================= */
namespace game
{

int initialize();
int processFrame(float timedelta, int width, int height, Input& input);
int destruct();

} // namespace game
