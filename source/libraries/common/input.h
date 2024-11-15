/**
 * @file input.h
 * @author Kevin Orbie
 * 
 * @brief Defines application user input data types.
 * @link Based on https://github.com/meemknight/windowsAPIforGamedevelopers/blob/master/day14/platform/include/input.h
 */

#ifndef INPUT_H
#define INPUT_H

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <stdint.h>
#include <stddef.h>

/* Standard C++ Libraries */
// None


/* ========================== Classes ========================== */
/**
 * @brief Defines a button.
 */
struct Button {
    uint8_t pressed  = 0;  // True on frame where key is first pressed.
    uint8_t held     = 0;  // True on all frames where key is down.
    uint8_t released = 0;  // True on frame where key is released.

    /* Link indexes to button names. */
    enum {
        SPACE = 0,
        T, C, F,
        A, W, S, D,
        LEFT, RIGHT, UP, DOWN,
        BUTTON_COUNT
    };

    /**
     * @brief Update state for this frame, based on if the button is down or not.
     * @param down Whether or not this button is in the down state for this frame.
     */
    void updateState(bool down) {
        if (down)
        { /* Button is DOWN in this frame. */
            if (!held) {
                pressed = 1;
            } else if(pressed == 1) {
                pressed = 0;
            }
            held = 1;
        } 
        else 
        { /* Button is UP in this frame. */
            if (held) {
                released = 1;
            } else if (released == 1){
                released = 0;
            }
            held = 0;
        }
    }
};

struct RawInput final {
    /* Key Inputs */
    Button keys[Button::BUTTON_COUNT];

    /* Mouse Inputs */
    float mouse_xpos;
    float mouse_ypos;
    float mouse_xoffset; 
    float mouse_yoffset; 
    float scroll_x_offset;
    float scroll_y_offset;

    Button left_mouse_btn;
    Button right_mouse_btn;
};

/**
 * @brief Converts raw inputs to logical actions.
 */
struct Input {
    bool car_backward = false;
    bool car_forward = false;
    bool car_right = false;
    bool car_left = false;

    float cam_move_xoffset = 0;
    float cam_move_yoffset = 0;
    float cam_zoom_offset = 0;
    bool cam_backward = false;
    bool cam_forward = false;
    bool cam_right = false;
    bool cam_left = false;

    bool cam_toggle_follow_mode = false;
    bool switch_cam_to_topview = false;

    bool drive_ctrl_active = false;
    bool drive_ctrl_updated = false;

    void update(const RawInput &input) {
        /* Drive Control. */
        bool tmp_car_backward   = input.keys[Button::DOWN].held;
        bool tmp_car_forward    = input.keys[Button::UP].held;
        bool tmp_car_right      = input.keys[Button::RIGHT].held;
        bool tmp_car_left       = input.keys[Button::LEFT].held;

        update(tmp_car_forward, tmp_car_backward, tmp_car_left, tmp_car_right);

        /* Cam Control. */
        cam_backward = input.keys[Button::S].held;
        cam_forward  = input.keys[Button::W].held;
        cam_right    = input.keys[Button::D].held;
        cam_left     = input.keys[Button::A].held;

        cam_toggle_follow_mode = input.keys[Button::F].pressed;
        switch_cam_to_topview = input.keys[Button::T].pressed;

        cam_move_xoffset = input.mouse_xoffset;
        cam_move_yoffset = input.mouse_yoffset;
        cam_zoom_offset = input.scroll_y_offset;
    }

    void update(bool new_car_fwd, bool new_car_back, bool new_car_left, bool new_car_right) {
        drive_ctrl_updated = (
            new_car_fwd != car_forward || 
            new_car_right != car_right || 
            new_car_left != car_left ||
            new_car_back != car_backward 
        );

        car_backward = new_car_back;
        car_forward = new_car_fwd;
        car_right = new_car_right;
        car_left = new_car_left;

        drive_ctrl_active = (car_forward || car_backward || car_left || car_right);
    }
};

// Inline to prevent multiple defintion error 
inline void zeroCallbackOffsets(RawInput& input) {
    input.mouse_xoffset   = 0; 
    input.mouse_yoffset   = 0; 
    input.scroll_x_offset = 0;
    input.scroll_y_offset = 0;
}

#endif
