#pragma once

namespace input
{

    constexpr u32 N_TOUCH_GESTURES = 8;
}


namespace input
{
    class TouchGesture
    {
    public:
        
        u64 device_id = 0;
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