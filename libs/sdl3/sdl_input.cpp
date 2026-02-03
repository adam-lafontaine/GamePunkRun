#include "../io/input/input_state.hpp"
#include "../math/math.hpp"
#include "sdl_include.hpp"


#define ASSERT_INPUT
#define LOG_INPUT


#ifndef NDEBUG

#ifdef LOG_INPUT
#define input_log(...) SDL_Log(__VA_ARGS__)
#else
#define input_log(...)
#endif

#ifdef ASSERT_INPUT
#define input_assert(condition) SDL_assert(condition)
#else
#define input_assert(...)
#endif

#else

#define input_log(...)
#define input_assert(...)

#endif


/* helpers */

namespace sdl
{

	static f32 normalize_axis_value(Sint16 axis)
    {
        constexpr math::MinMax<Sint16> mm_axis = {
            .min = SDL_JOYSTICK_AXIS_MIN,
            .max = SDL_JOYSTICK_AXIS_MAX
        };

        constexpr math::MinMax<f32> mm_res = {
            .min = -1.0f,
            .max = 1.0f
        };

        f32 norm = math::cxpr::lerp(axis, mm_axis, mm_res);

        return math::abs(norm) < 0.3f ? 0.0f : norm;
    }


    static void handle_sdl_event(SDL_Event const& event, input::Input& input)
    {
        static bool is_fullscreen = false;

        SDL_Keycode key_code;
        SDL_Scancode scan_code;

        switch (event.type)
        {
        case SDL_EVENT_WINDOW_RESIZED:
            input.window_size_changed = 1;
            break;

        case SDL_EVENT_QUIT:
            print_message("SDL_QUIT");
            input.cmd_end_program = 1;
            break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        {
            key_code = event.key.key;
            scan_code = event.key.scancode;

            auto alt = event.key.mod & SDL_KMOD_ALT;

            if (alt)
            {
                switch (key_code)
                {
                case SDLK_F4:
                    print_message("ALT F4");
                    input.cmd_end_program = 1;
                    break;

                #ifndef NDEBUG
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    print_message("ALT ENTER");
                    is_fullscreen = !is_fullscreen;                    
                    SDL_SetWindowFullscreen(SDL_GetWindowFromID(event.window.windowID), is_fullscreen);
                    input.window_size_changed = 1;
                    break;
                #endif
                }
            }
            
            switch (key_code)
            {

            #ifndef NDEBUG
            case SDLK_ESCAPE:
                print_message("ESC");
                input.cmd_end_program = 1;
                break;

            #endif


            default:
                break;
            }
            

        } break;
        }
    }
}


#include "sdl_input_joystick.cpp"
#include "sdl_input_keyboard.cpp"
#include "sdl_input_mouse.cpp"
#include "sdl_input_touch.cpp"


/* api */

namespace input
{
    static constexpr Uint32 subsystem_flags()
    {
        constexpr auto gamepad = (N_GAMEPAD_BUTTONS > 0 || N_GAMEPAD_AXES > 0) ? SDL_INIT_GAMEPAD : 0u;
        constexpr auto joystick = (N_JOYSTICK_BUTTONS > 0 || N_JOYSTICK_AXES > 0) ? SDL_INIT_JOYSTICK : 0u;

        

        return gamepad | joystick;
    }


    bool init(InputArray& inputs)
    {
        reset_input_state(inputs.prev());
        reset_input_state(inputs.curr());

        if (!SDL_InitSubSystem(subsystem_flags()))
        {
            sdl::print_error("Init Input failed");
            return false;
        }

        sdl::open_device_list(inputs);

        return true;
    }


    void close()
    {
        sdl::close_device_list();
        SDL_QuitSubSystem(subsystem_flags());
    }


    void record_input(InputArray& inputs)
    {
        auto& prev = inputs.prev();
        auto& curr = inputs.curr();

        copy_input_state(prev, curr);
        curr.frame = prev.frame + 1;
        curr.dt_frame = 1.0f / 60.0f; // TODO
        curr.flags = 0;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            sdl::handle_sdl_event(event, curr);
            sdl::update_device_list(event, inputs);

            sdl::record_keyboard_input_event(event, prev, curr);
            sdl::record_mouse_input_event(event, prev, curr);
            sdl::record_gamepad_input_event(event, prev, curr);
            sdl::record_joystick_input_event(event, prev, curr);
            sdl::record_touch_input_event(event, prev, curr);
        }

        sdl::record_gamepad_axes(curr);
        sdl::record_joystick_axes(curr);
    }


    void record_input(InputArray& inputs, event_cb handle_event)
    {
        auto& prev = inputs.prev();
        auto& curr = inputs.curr();

        copy_input_state(prev, curr);
        curr.frame = prev.frame + 1;
        curr.dt_frame = 1.0f / 60.0f; // TODO
        curr.flags = 0;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            //sdl::handle_sdl_event(event, curr);
            handle_event(&event);
            sdl::update_device_list(event, inputs);

            sdl::record_keyboard_input_event(event, prev, curr);
            sdl::record_mouse_input_event(event, prev, curr);
            sdl::record_gamepad_input_event(event, prev, curr);
            sdl::record_joystick_input_event(event, prev, curr);
            sdl::record_touch_input_event(event, prev, curr);
        }

        sdl::record_gamepad_axes(curr);
        sdl::record_joystick_axes(curr);
    }
}