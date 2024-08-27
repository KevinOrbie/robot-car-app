/**
 * @brief Game Input Data.
 * @author Kevin Orbie
 * @link Based on https://github.com/meemknight/windowsAPIforGamedevelopers/blob/master/day14/platform/include/input.h
 */

#ifndef INPUT_H
#define INPUT_H

/* ========================== Include ========================== */
/* C/C++ Libraries */
#include <stdint.h>


/* ========================== Classes ========================== */
struct Button {
    uint8_t pressed  = 0;  // True on frame where key is first pressed
    uint8_t held     = 0;  // True on all frames where key is down
    uint8_t released = 0;  // True on frame where key is released

    // Link indexes to button names
    enum {
        SPACE = 0,
        A, W, S, D,
        BUTTON_COUNT
    };

    /**
     * @brief Update state for this frame, based on if the button is down or not.
     * @param down Whether or not this button is in the down state for this frame.
     */
    void updateState(bool down) {
        if (down)
        { /* Button is DOWN in this frame. */
            if (held) {
                pressed = 0;
            } else {
                pressed = 1;
            }
            held = 1;
        } 
        else 
        { /* Button is UP in this frame. */
            if (held) {
                released = 1;
            } else {
                released = 0;
            }
            held = 0;
        }
    }
};

struct Input 
{
    // Key Inputs
    Button keys[Button::BUTTON_COUNT];

    // Mouse Inputs
    float mouse_xpos;
    float mouse_ypos;
    float mouse_xoffset; 
    float mouse_yoffset; 
    float scroll_x_offset;
    float scroll_y_offset;

    Button left_mouse_btn;
    Button right_mouse_btn;
};

// Inline to prevent multiple defintion error 
inline void zeroCallbackOffsets(Input& input){
    input.mouse_xoffset   = 0; 
    input.mouse_yoffset   = 0; 
    input.scroll_x_offset = 0;
    input.scroll_y_offset = 0;
}

#endif
