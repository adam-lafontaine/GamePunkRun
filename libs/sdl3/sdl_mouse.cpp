/* mouse */

namespace sdl
{
#ifndef NO_MOUSE
    using MouseInput = input::MouseInput;


    static void record_mouse_button_input(Uint8 button_code, MouseInput const& old_mouse, MouseInput& new_mouse, bool is_down)
    {
        switch(button_code)
        {		
        #if MOUSE_LEFT
            case SDL_BUTTON_LEFT:
            {
                record_button_input(old_mouse.btn_left, new_mouse.btn_left, is_down);
            } break;
        #endif
        #if MOUSE_RIGHT
            case SDL_BUTTON_RIGHT:
            {
                record_button_input(old_mouse.btn_right, new_mouse.btn_right, is_down);
            } break;
        #endif
        #if MOUSE_MIDDLE
            case SDL_BUTTON_MIDDLE:
            {
                record_button_input(old_mouse.btn_middle, new_mouse.btn_middle, is_down);
            } break;
        #endif
        #if MOUSE_X1
            case SDL_BUTTON_X1:
            {
                record_button_input(old_mouse.btn_x1, new_mouse.btn_x1, is_down);
            } break;
        #endif
        #if MOUSE_X2
            case SDL_BUTTON_X2:
            {
                record_button_input(old_mouse.btn_x1, new_mouse.btn_x1, is_down);
            } break;
        #endif
        }
    }


    static void record_mouse_position_input(MouseInput& mouse, SDL_MouseMotionEvent const& motion)
    {
    #if MOUSE_POSITION
        mouse.window_pos.x = motion.x;
        mouse.window_pos.y = motion.y;
    #endif
    }


    static void record_mouse_wheel_input(MouseInput& mouse, SDL_MouseWheelEvent const& wheel)
    {
    #if MOUSE_WHEEL
        mouse.wheel.x = wheel.x;
        mouse.wheel.y = wheel.y;
    #endif
    }


#endif


    static void record_mouse_input_event(SDL_Event const& event, MouseInput const& old_mouse, MouseInput& new_mouse)
    {
    #ifndef NO_MOUSE

        auto& mouse = new_mouse;

        switch (event.type)
        {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            bool is_down = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
            auto button_code = event.button.button;

            mouse.window_id = event.button.windowID;

            record_mouse_button_input(button_code, old_mouse, mouse, is_down);
        } break;

        #if MOUSE_POSITION
        case SDL_EVENT_MOUSE_MOTION:
        { 
            mouse.window_id = event.motion.windowID;
            record_mouse_position_input(mouse, event.motion); 
        } break;
        #endif

        #if MOUSE_WHEEL
        case SDL_EVENT_MOUSE_WHEEL:
        { 
            mouse.window_id = event.wheel.windowID;
            record_mouse_wheel_input(mouse, event.wheel);
        } break;
        #endif
            
        }

    #endif
    }
}