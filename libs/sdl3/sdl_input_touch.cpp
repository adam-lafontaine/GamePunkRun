/* touch gesture */

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
            if (!item.is_active)
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


    static void record_touch_gesture(SDL_Event const& event, TouchInput const& prev, TouchInput& curr)
    {
    #ifndef NO_TOUCH

        bool is_down = false;
        SDL_TouchID device = 0;
        SDL_FingerID gesture = 0;

        switch (event.type)
        {
        case SDL_EVENT_FINGER_DOWN:
        {
            device = event.tfinger.touchID;
            gesture = event.tfinger.fingerID;
            auto id = spawn_gesture(curr, device, gesture);
            if (id < 0)
            {
                return;
            }

            is_down = true;

            auto& p = prev.gestures[id];
            auto& c = curr.gestures[id];

            input::record_button_input(p.btn_touch, c.btn_touch, is_down);

            c.pos.x = event.tfinger.x;
            c.pos.y = event.tfinger.y;                

        } break;

        case SDL_EVENT_FINGER_UP:
        { 
            device = event.tfinger.touchID;
            gesture = event.tfinger.fingerID;
            auto id = find_gesture(curr, device, gesture);
            if (id < 0)
            {
                return;
            }

            is_down = false;

            auto& p = prev.gestures[id];
            auto& c = curr.gestures[id];

            input::record_button_input(p.btn_touch, c.btn_touch, is_down);

            c.pos.x = event.tfinger.x;
            c.pos.y = event.tfinger.y;

            // despawn next frame
            c.device_id = 0;

        } break;

        case SDL_EVENT_FINGER_MOTION:
        { 
            device = event.tfinger.touchID;
            gesture = event.tfinger.fingerID;
            auto id = find_gesture(curr, device, gesture);
            if (id < 0)
            {
                return;
            }

            auto& c = curr.gestures[id];

            c.pos.x = event.tfinger.x;
            c.pos.y = event.tfinger.y;

        } break;

        default:
            break;
        }

    #endif
    }
    
}


/* touch joystick */

namespace sdl
{
#ifdef TOUCH_JOYSTICK

    class TouchJoystickButtonList
    {
    public:
        static constexpr u32 capacity = input::N_JOYSTICK_BUTTONS;

        u32 size = 0;

        b8 is_set[capacity] = { 0 };
        ButtonCode buttons[capacity];
        Rect2Df32 regions[capacity];
        SDL_FingerID gesture_ids[capacity] = { 0 };
    };


    static TouchJoystickButtonList touch_joystick_buttons;


    static void set_touch_joystick_btn_region(ButtonCode btn, Rect2Df32 region)
    {
        auto& list = touch_joystick_buttons;
        auto S = list.size;

        u32 i = 0;
        for (; i < S; i++)
        {
            if (list.is_set && list.buttons[i] == btn)
            {
                break;
            }
        }

        if (i < S)
        {
            list.regions[i] = region;
            return;
        }

        list.is_set[i] = 1;
        list.buttons[i] = btn;
        list.regions[i] = region;
        list.size++;
    }


    static ButtonCode find_touch_joystick_btn(Point2Df32 pos)
    {
        auto const contains = [](Point2Df32 p, Rect2Df32 r)
        {
            return
                p.x >= r.x_begin &&
                p.x < r.x_end &&
                p.y >= r.y_begin &&
                p.y < r.y_end;
        };

        auto& list = touch_joystick_buttons;
        auto S = list.size;

        for (u32 i = 0; i < S; i++)
        {
            if (list.is_set[i] && contains(pos, list.regions[i]))
            {
                return list.buttons[i];
            }
        }

        return 255;
    }

#endif


    static void record_touch_joystick(SDL_Event const& event, JoystickInput const& prev, JoystickInput& curr)
    {
    #ifdef TOUCH_JOYSTICK

        bool is_down = false;
        ButtonCode btn = 255;
        SDL_TouchID device = 0;
        SDL_FingerID gesture = 0;

        switch (event.type)
        {
        case SDL_EVENT_FINGER_DOWN:
        case SDL_EVENT_FINGER_UP:
        {
            is_down = event.type == SDL_EVENT_FINGER_DOWN;
            device = event.tfinger.touchID;
            gesture = event.tfinger.fingerID;

        } break;

        case SDL_EVENT_FINGER_MOTION:
        {
            is_down = true;
            device = event.tfinger.touchID;
            gesture = event.tfinger.fingerID;


        } break;

        default:
            break;
        }

    #endif
    }
}


namespace sdl
{
    static void record_touch_input_event(SDL_Event const& event, Input const& prev, Input& curr)
    {
        record_touch_gesture(event, prev.touch, curr.touch);
    }
}


/* touch joystick api */

namespace input
{
#ifdef TOUCH_JOYSTICK

#if JOYSTICK_BTN_0
    void set_touch_joystick_btn_0(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(0, region); }
#endif

#if JOYSTICK_BTN_1
    void set_touch_joystick_btn_1(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(1, region); }
#endif

#if JOYSTICK_BTN_2
    void set_touch_joystick_btn_2(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(2, region); }
#endif

#if JOYSTICK_BTN_3
    void set_touch_joystick_btn_3(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(3, region); }
#endif

#if JOYSTICK_BTN_4
    void set_touch_joystick_btn_4(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(4, region); }
#endif

#if JOYSTICK_BTN_5
    void set_touch_joystick_btn_5(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(5, region); }
#endif

#if JOYSTICK_BTN_6
    void set_touch_joystick_btn_6(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(6, region); }
#endif

#if JOYSTICK_BTN_7
    void set_touch_joystick_btn_7(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(7, region); }
#endif

#if JOYSTICK_BTN_8
    void set_touch_joystick_btn_8(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(8, region); }
#endif

#if JOYSTICK_BTN_9
    void set_touch_joystick_btn_9(Rect2Df32 region) { sdl::set_touch_joystick_btn_region(9, region); }
#endif


#endif
}