#pragma once

#include "input.hpp"

namespace input
{
    

    constexpr u32 N_TOUCH_FINGERS = 8;
}


namespace input
{
    class TouchInput
    {
    public:
        
        u64 device_id = 0;
        u64 gesture_id = 0;

        Point2Di32 pos = { 0 };

        ButtonState touch;
    };
}