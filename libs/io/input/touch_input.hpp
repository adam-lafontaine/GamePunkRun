#pragma once

#define TOUCH_JOYSTICK 1
//#define TOUCH_GAMEPAD 1

namespace input
{

    constexpr u32 N_TOUCH_GESTURES = 8;
}


namespace input
{
    class TouchGesture
    {
    public:
        
        union 
        {
            u64 device_id = 0;
            u64 is_active;
        };
        
        u64 gesture_id = 0;

        Point2Df32 pos = { 0 };

        ButtonState btn_touch;
    };


    class TouchInput
    {
    public:
        static constexpr u32 count = N_TOUCH_GESTURES;

        TouchGesture gestures[count];
    };
}


/* map touch joystick */

namespace input
{
#ifdef TOUCH_JOYSTICK

#if JOYSTICK_BTN_0
    void set_touch_joystick_btn_0(Rect2Df32 region);
#endif

#if JOYSTICK_BTN_1
    void set_touch_joystick_btn_1(Rect2Df32 region);
#endif

#if JOYSTICK_BTN_2
    void set_touch_joystick_btn_2(Rect2Df32 region);
#endif

#if JOYSTICK_BTN_3
    void set_touch_joystick_btn_3(Rect2Df32 region);
#endif

#if JOYSTICK_BTN_4
    void set_touch_joystick_btn_4(Rect2Df32 region);
#endif

#if JOYSTICK_BTN_5
    void set_touch_joystick_btn_5(Rect2Df32 region);
#endif

#if JOYSTICK_BTN_6
    void set_touch_joystick_btn_6(Rect2Df32 region);
#endif

#if JOYSTICK_BTN_7
    void set_touch_joystick_btn_7(Rect2Df32 region);
#endif

#if JOYSTICK_BTN_8
    void set_touch_joystick_btn_8(Rect2Df32 region);
#endif

#if JOYSTICK_BTN_9
    void set_touch_joystick_btn_9(Rect2Df32 region);
#endif


#endif
}