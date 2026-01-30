

#include "../io/input/input.hpp"


/* touch */

namespace sdl
{
#ifndef NO_TOUCH

    using TouchInput = input::TouchInput;

#endif
}


namespace sdl
{
    static void record_touch_input_event(SDL_Event const& event, Input const& prev, Input& curr)
    {
    #ifndef NO_TOUCH

        bool is_down = false;


        switch (event.type)
        {
        case SDL_EVENT_FINGER_DOWN:
        case SDL_EVENT_FINGER_UP:
            is_down = event.type == SDL_EVENT_FINGER_DOWN;
            
            SDL_TouchID x;
            SDL_FingerID b;
            break;

        case SDL_EVENT_FINGER_MOTION:

            break;

        default:
            break;
        }

    #endif
    }
}