#include "../../../../libs/io/window.hpp"
#include "../../../../libs/io/input/input.hpp"
#include "../../../../libs/datetime/datetime.hpp"
#include "../../../../libs/math/math.hpp"

#include "../../app/app.hpp"

#include <emscripten.h>
#include <cstdio>
#include <cstdlib>

namespace mb = memory_buffer;
namespace img = image;
namespace dt = datetime;
namespace game = game_punk;


constexpr auto WINDOW_TITLE = game::APP_TITLE;
//constexpr f32 WINDOW_SCALE = 0.5f;

constexpr f64 NANO = 1'000'000'000;
constexpr f64 MICRO = 1'000'000;

constexpr f64 TARGET_FRAMERATE_HZ = 60.0;
constexpr f64 TARGET_NS_PER_FRAME = NANO / TARGET_FRAMERATE_HZ;
constexpr f64 TARGET_MS_PER_FRAME = TARGET_NS_PER_FRAME / MICRO;

using Stopwatch = dt::StopwatchNS;



class EmControllerState
{
public:
    b8 dpad_up = 0;
    b8 dpad_down = 0;
    b8 dpad_left = 0;
    b8 dpad_right = 0;
    b8 btn_a = 0;
    b8 btn_b = 0;
    b8 btn_x = 0;
    b8 btn_y = 0;    
};


union EmState
{
    u32 state = 0;

    struct
    {
        u32 has_console:1;
        u32 has_gamepad:1;
        u32 has_dpad_up:1;
        u32 has_dpad_down:1;
        u32 has_dpad_left:1;
        u32 has_dpad_right:1;
        u32 has_btn_a:1;
        u32 has_btn_b:1;
        u32 has_btn_x:1;
        u32 has_btn_y:1;
    };
};


static void read_controller_state(EmControllerState state, input::GamepadInput const& prev, input::GamepadInput& curr)
{
    auto const record_button_state = [](auto const& old_state, auto& new_state, b8 is_down)
    {
        new_state.pressed = !old_state.is_down && is_down;
		new_state.is_down = is_down;
		new_state.raised = old_state.is_down && !is_down;
    };

    record_button_state(prev.btn_dpad_up, curr.btn_dpad_up, state.dpad_up);
    record_button_state(prev.btn_dpad_down, curr.btn_dpad_down, state.dpad_down);
    record_button_state(prev.btn_dpad_left, curr.btn_dpad_left, state.dpad_left);
    record_button_state(prev.btn_dpad_right, curr.btn_dpad_right, state.dpad_right);
    record_button_state(prev.btn_south, curr.btn_south, state.btn_a);
    record_button_state(prev.btn_east, curr.btn_east, state.btn_b);
    record_button_state(prev.btn_west, curr.btn_west, state.btn_x);
    record_button_state(prev.btn_north, curr.btn_north, state.btn_y);
}


/* static main variables */

enum class RunState : int
{
    Begin,
    Run,
    Error,
    End
};


namespace mv
{
    constexpr int MAIN_ERROR = 1;
    constexpr int MAIN_OK = 0;

    constexpr u32 GAME_SCALE = 2;

    RunState run_state = RunState::Begin;

    window::Window window;
    input::InputArray input;

    EmControllerState em_controller{};

    game::AppState app_state;
    Stopwatch frame_sw;

#ifdef APP_ROTATE_90
    constexpr window::Rotate GAME_ROTATE = window::Rotate::CounterClockwise_90;
#endif
}


int update_em_controller(char btn, int is_down)
{
    b8 btn_down = (is_down > 0) ? 1 : 0;

    int res = btn;

    switch (btn)
    {

    case 'u':
    case 'U':
        mv::em_controller.dpad_up = btn_down;
        break;
    
    case 'd':
    case 'D':
        mv::em_controller.dpad_down = btn_down;
        break;

    case 'l':
    case 'L':
        mv::em_controller.dpad_left = btn_down;
        break;

    case 'r':
    case 'R':
        mv::em_controller.dpad_right = btn_down;
        break;

    case 'a':
    case 'A':
        mv::em_controller.btn_a = btn_down;
        break;

    case 'b':
    case 'B':
        mv::em_controller.btn_b = btn_down;
        break;

    case 'x':
    case 'X':
        mv::em_controller.btn_x = btn_down;
        break;

    case 'y':
    case 'Y':
        mv::em_controller.btn_y = btn_down;
        break;

    default: return -1;
    }

    return res;
}


void end_program()
{
    mv::run_state = RunState::End;
}


static inline bool is_running()
{
    return mv::run_state != RunState::End;
}


static void cap_framerate()
{
    constexpr f64 fudge = 0.9;

    auto ns = mv::frame_sw.get_time_nano_f64();

    auto sleep_ns = TARGET_NS_PER_FRAME - ns;
    if (sleep_ns > 0.0)
    {
        dt::delay_nano((u64)(sleep_ns * fudge));
    }
    
    mv::frame_sw.start();
}


class InitParams
{
public:
    u32 max_width = 0;
    u32 max_height = 0;

    b32 is_mobile = 0;
};


static bool window_create(Vec2Du32 game_dims, InitParams const& params)
{ 
    auto game_w = game_dims.x;
    auto game_h = game_dims.y;

    auto max_w = params.max_width ? params.max_width : game_w * mv::GAME_SCALE;
    auto max_h = params.max_height ? params.max_height : game_h * mv::GAME_SCALE;

    auto scale_w = (f32)max_w / game_w;
    auto scale_h = (f32)max_h / game_h;

    auto scale = math::min(scale_w, scale_h);

#ifdef APP_ROTATE_90

    // rotated
    auto w = math::cxpr::round_to_unsigned<u32>(scale * game_dims.y);
    auto h = math::cxpr::round_to_unsigned<u32>(scale * game_dims.x);

    Vec2Du32 window_dims = { w, h };

    return window::create(mv::window, game::APP_TITLE, window_dims, game_dims, mv::GAME_ROTATE);

#else
    
    auto w = math::cxpr::round_to_unsigned<u32>(scale * game_dims.x);
    auto h = math::cxpr::round_to_unsigned<u32>(scale * game_dims.y);

    Vec2Du32 window_dims = { w, h };

    return window::create(mv::window, game::APP_TITLE, window_dims, game_dims);

#endif
}


static void window_render(b8 window_size_changed)
{
#ifdef APP_ROTATE_90
    window::render(mv::window, mv::GAME_ROTATE, window_size_changed);
#else
    window::render(mv::window, window_size_changed);
#endif
}


img::ImageView make_window_view()
{
    static_assert(window::PIXEL_SIZE == sizeof(img::Pixel));

    img::ImageView view{};
    view.matrix_data_ = (img::Pixel*)mv::window.pixel_buffer;
    view.width = mv::window.width_px;
    view.height = mv::window.height_px;

    return view;
}


static bool main_init(InitParams const& params)
{
#ifndef NDEBUG
    auto ts = dt::current_timestamp_i64();

    printf("0x%llx\n", ts);
#endif    
    
    if (!window::init())
    {
        printf("Window error\n");
        return false;
    }

    if (!input::init(mv::input))
    {
        printf("Input error\n");
        return false;
    }

    auto w = params.max_width;
    auto h = params.max_height;

    Vec2Du32 dims = { w, h };

    //auto result = game::init(mv::app_state, dims);
    auto result = game::init(mv::app_state);
    if (!result.success)
    {
        printf("%s\n", game::decode_error(result.error));
        return false;
    }

    if (!window_create(result.app_dimensions, params))
    {
        return false;
    }

    auto app_screen = make_window_view();

    if (!game::set_screen_memory(mv::app_state, app_screen))
    {
        return false;
    }

    return true;
}


void main_close()
{
    mv::run_state = RunState::End;

    game::close(mv::app_state);
    input::close();
    window::close();
}


static void main_loop()
{
    input::record_input(mv::input);
    auto& input = mv::input.curr();
    auto& prev = mv::input.prev();

    if (input.cmd_end_program)
    {
        end_program();
    }

    read_controller_state(mv::em_controller, prev.gamepads[0], input.gamepads[0]);

    game::update(mv::app_state, input);

    window_render(input.window_size_changed);

    if (!is_running())
    {
        emscripten_cancel_main_loop();
    }

    mv::input.swap();
    cap_framerate();
}


static void print_controls()
{
    constexpr auto str = ""
    " ___________________________________________________\n"
    "|                          | Gamepad  | Keyboard    |\n" 
    "| Gameplay Controls        | (mobile) | (desktop)   |\n" 
    "|__________________________|__________|_____________|\n"
    "| Action/Change animation  | A        | Spacebar    |\n"
    "|__________________________|__________|_____________|\n"
    "\n"
    " ___________________________________________________\n"
    "|                          | Gamepad  | Keyboard    |\n" 
    "| Camera Controls          | (mobile) | (desktop)   |\n" 
    "|__________________________|__________|_____________|\n" 
    "| Move Right               | Right    | Arrow Right |\n"
    "| Move Left                | Left     | Arrow Left  |\n"
    "| Move Up                  | Up       | Arrow Up    |\n"
    "| Move Down                | Down     | Arrow Down  |\n"
    "|__________________________|__________|_____________|\n"
    ;

    printf(str);
}


static void print_credits()
{
    constexpr auto str = 
    "\n----------------------------------------------------\n"
    "Pixel art:\n"
    "https://itch.io/c/2053292/cyberpunk-pixel-art / https://craftpix.net/\n"
    "\n----------------------------------------------------\n"
    /*"Music (CCO)\n"
    "https://opengameart.org/content/space-5\n" // intro
    "https://opengameart.org/content/leap-8bit\n" // 03
    "https://opengameart.org/content/boss-battle-1-8-bit-re-upload\n" // 00
    "https://opengameart.org/content/boss-battle-2-8-bit-re-upload\n" // 02
    "https://opengameart.org/content/boss-battle-3-8-bit-re-upload\n" // 01    
    "\n----------------------------------------------------\n"*/
    "Programming:\n"
    "Adam Lafontaine\n"    
    ;

    printf(str);
}


static void print_messages()
{

}


int main(int argc, char* argv[])
{
    InitParams params{};

    auto const get_arg = [&](int idx) 
    { 
        auto arg = std::atoi(argv[idx]);
        return arg > 0 ? (u32)arg : (u32)0;
    };

    switch (argc)
    {
    case 0:
    case 1:        
        break;

    case 2:
        params.is_mobile = get_arg(1);
        break;

    case 3:
        params.max_width = get_arg(1);
        params.max_height = get_arg(2);
        break;

    default:
        params.max_width = get_arg(1);
        params.max_height = get_arg(2);
        params.is_mobile = get_arg(3);
        break;
    }

    printf("\n%s v%s | %s\n\n", game::APP_TITLE, game::VERSION, game::DATE);

    

    if (!main_init(params))
    {
        return EXIT_FAILURE;
    }

    print_controls();
    print_credits();
    print_messages();

    emscripten_set_main_loop(main_loop, 0, 1);

    mv::run_state = RunState::Run;
    
    main_close();

    return EXIT_SUCCESS;
}


extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    void kill()
    {
        end_program();
    }


    EMSCRIPTEN_KEEPALIVE
    int gamepad_button(char btn, int is_down)
    {
        return update_em_controller(btn, is_down);
    }
    

    EMSCRIPTEN_KEEPALIVE
    u32 get_state()
    {
        EmState state{};

        state.has_console   = 1;
        state.has_gamepad   = 1;
        state.has_dpad_up    = 1;
        state.has_dpad_down  = 1;
        state.has_dpad_left  = 1;
        state.has_dpad_right = 1;
        state.has_btn_a     = 1;
        state.has_btn_b     = 0;
        state.has_btn_x     = 0;
        state.has_btn_y     = 0;

        return state.state;
    }
}


#include "../main_o/main_o_sdl2.cpp"