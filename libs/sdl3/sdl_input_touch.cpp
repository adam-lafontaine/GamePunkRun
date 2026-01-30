/* touch */

namespace sdl
{
#ifndef NO_TOUCH

    using TouchInput = input::TouchInput;
    using TouchGesture = input::TouchGesture;


    static i32 spawn_gesture(TouchInput& touch, SDL_TouchID d, SDL_FingerID g)
    {
        constexpr auto N = (i32)TouchInput::count;

        for (i32 i = 0; i < N; i++)
        {
            auto& item = touch.gestures[i];
            if (!item.device_id)
            {
                item.device_id = d;
                item.gesture_id = g;
                item.pos = { 0 };
                item.btn_touch.any = 0;
                return i;
            }
        }

        return -1;
    }


    static i32 find_gesture(TouchInput& touch, SDL_TouchID d, SDL_FingerID g)
    {
        constexpr auto N = (i32)TouchInput::count;

        for (i32 i = 0; i < N; i++)
        {
            auto& item = touch.gestures[i];
            if (item.device_id == d && item.gesture_id == g)
            {
                return i;
            }
        }

        return -1;
    }

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
        {
            auto id = spawn_gesture(curr.touch, event.tfinger.touchID, event.tfinger.fingerID);
            if (id < 0)
            {
                return;
            }

            auto& p = prev.touch.gestures[id];
            auto& c = curr.touch.gestures[id];

            input::record_button_input(p.btn_touch, c.btn_touch, is_down);

            c.pos.x = event.tfinger.x;
            c.pos.y = event.tfinger.y;                

        } break;

        case SDL_EVENT_FINGER_UP:
        { 
            auto id = find_gesture(curr.touch, event.tfinger.touchID, event.tfinger.fingerID);
            if (id < 0)
            {
                return;
            }

            is_down = false;

            auto& p = prev.touch.gestures[id];
            auto& c = curr.touch.gestures[id];

            input::record_button_input(p.btn_touch, c.btn_touch, is_down);

            c.pos.x = event.tfinger.x;
            c.pos.y = event.tfinger.y;

            // despawn next frame
            c.device_id = 0;

        } break;

        case SDL_EVENT_FINGER_MOTION:
        { 
            auto id = find_gesture(curr.touch, event.tfinger.touchID, event.tfinger.fingerID);
            if (id < 0)
            {
                return;
            }

            auto& c = curr.touch.gestures[id];

            c.pos.x = event.tfinger.x;
            c.pos.y = event.tfinger.y;

        } break;

        default:
            break;
        }

    #endif
    }
}