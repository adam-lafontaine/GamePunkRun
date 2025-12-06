#include "../io/input/input_state.hpp"
#include "../util/numeric.hpp"
#include "sdl_include.hpp"


namespace num = numeric;


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
        constexpr num::MinMax<Sint16> mm_axis = {
            .min = SDL_JOYSTICK_AXIS_MIN,
            .max = SDL_JOYSTICK_AXIS_MAX
        };

        constexpr num::MinMax<f32> mm_res = {
            .min = -1.0f,
            .max = 1.0f
        };

        f32 norm = num::lerp(axis, mm_axis, mm_res);

        return num::abs(norm) < 0.3f ? 0.0f : norm;
    }


    static void set_vector_state(input::VectorState<f32>& vs)
    {
        auto& vec = vs.vec;
        auto& unit = vs.unit;

        vs.magnitude = num::magnitude(vec);

        auto mag = vs.magnitude > 0.0f ? vs.magnitude : 1.0f;

        unit.x = vec.x / mag;
        unit.y = vec.y / mag;
    }


    static void set_unit_vector_state(input::VectorState<i8>& vs, int x, int y)
    {
        auto& vec = vs.vec;
        auto& unit = vs.unit;

        vec.x = num::sign_i8(x);
        vec.y = num::sign_i8(y);

        unit.x = (f32)vec.x;
        unit.y = (f32)vec.y;

        constexpr f32 hypot = 1.4142135f;
        constexpr f32 i_hypot = 1.0f / hypot;
        
        auto mag = (x || y) ? 1.0f : 0.0f;
        auto i_mag = (x && y) ? i_hypot : (x || y) ? 1.0f : 0.0f;

        vs.magnitude = mag;
        unit.x *= i_mag;
        unit.y *= i_mag;
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


#include "sdl_joystick.cpp"
#include "sdl_keyboard.cpp"
#include "sdl_mouse.cpp"


/* api */

namespace input
{
    static constexpr Uint32 subsystem_flags()
    {
        auto gamepad = (N_GAMEPAD_BUTTONS || N_GAMEPAD_AXES) ? SDL_INIT_GAMEPAD : 0u;
        auto joystick = (N_JOYSTICK_BUTTONS || N_JOYSTICK_AXES) ? SDL_INIT_JOYSTICK : 0u;

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
            sdl::record_keyboard_input_event(event, prev.keyboard, curr.keyboard);
            sdl::record_mouse_input_event(event, prev.mouse, curr.mouse);
            sdl::update_device_list(event, inputs);
            sdl::record_gamepad_input_event(event, prev, curr);
            sdl::record_joystick_input_event(event, prev, curr);
        }

        sdl::record_gamepad_axes(curr);
        sdl::record_joystick_axes(curr);

        set_is_active(curr);
        sdl::set_gamepad_vector_states(curr);
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
            sdl::record_keyboard_input_event(event, prev.keyboard, curr.keyboard);
            sdl::record_mouse_input_event(event, prev.mouse, curr.mouse);
            sdl::update_device_list(event, inputs);
            sdl::record_gamepad_input_event(event, prev, curr);
            sdl::record_joystick_input_event(event, prev, curr);
        }

        sdl::record_gamepad_axes(curr);
        sdl::record_joystick_axes(curr);

        set_is_active(curr);
        sdl::set_gamepad_vector_states(curr);
    }
}