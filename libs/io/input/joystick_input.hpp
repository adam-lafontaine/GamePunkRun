#pragma once

//#define NO_JOYSTICK
//#define SINGLE_JOYSTICK

#define JOYSTICK_BTN_0 1
#define JOYSTICK_BTN_1 1
#define JOYSTICK_BTN_2 1
#define JOYSTICK_BTN_3 1
#define JOYSTICK_BTN_4 1
#define JOYSTICK_BTN_5 1
#define JOYSTICK_BTN_6 0
#define JOYSTICK_BTN_7 0
#define JOYSTICK_BTN_8 1
#define JOYSTICK_BTN_9 1

#define JOYSTICK_AXIS_0 1
#define JOYSTICK_AXIS_1 1
#define JOYSTICK_AXIS_2 1
#define JOYSTICK_AXIS_3 1
#define JOYSTICK_AXIS_4 1
#define JOYSTICK_AXIS_5 1


namespace input
{

#ifdef NO_JOYSTICK
    constexpr unsigned N_JOYSTICK_BUTTONS = 0;
    constexpr unsigned N_JOYSTICK_AXES = 0;
#else

    constexpr unsigned N_JOYSTICK_BUTTONS = 
    JOYSTICK_BTN_0 +
    JOYSTICK_BTN_1 +
    JOYSTICK_BTN_2 +
    JOYSTICK_BTN_3 +
    JOYSTICK_BTN_4 +
    JOYSTICK_BTN_5 +
    JOYSTICK_BTN_6 +
    JOYSTICK_BTN_7 +
    JOYSTICK_BTN_8 +
    JOYSTICK_BTN_9;

    constexpr unsigned N_JOYSTICK_AXES = 
    JOYSTICK_AXIS_0 +
    JOYSTICK_AXIS_1 +
    JOYSTICK_AXIS_2 +
    JOYSTICK_AXIS_3 +
    JOYSTICK_AXIS_4 +
    JOYSTICK_AXIS_5;

#endif
}
