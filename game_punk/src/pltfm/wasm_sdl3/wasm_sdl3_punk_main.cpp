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

constexpr f64 NANO = 1'000'000'000;
constexpr f64 MICRO = 1'000'000;

constexpr f64 TARGET_FRAMERATE_HZ = 60.0;
constexpr f64 TARGET_NS_PER_FRAME = NANO / TARGET_FRAMERATE_HZ;
constexpr f64 TARGET_MS_PER_FRAME = TARGET_NS_PER_FRAME / MICRO;

using Stopwatch = dt::StopwatchNS;


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

    //EmControllerState em_controller{};

    game::AppState app_state;
    Stopwatch frame_sw;

#ifdef APP_ROTATE_90
    constexpr window::Rotate GAME_ROTATE = window::Rotate::CounterClockwise_90;
#endif
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
#ifdef APP_ROTATE_90

    auto game_w = game_dims.y;
    auto game_h = game_dims.x;

#else

    auto game_w = game_dims.x;
    auto game_h = game_dims.y;

#endif    

    auto max_w = params.max_width ? params.max_width : game_w * mv::GAME_SCALE;
    auto max_h = params.max_height ? params.max_height : game_h * mv::GAME_SCALE;

    auto scale_w = (f32)max_w / game_w;
    auto scale_h = (f32)max_h / game_h;

    // assume mobile screen height is greater
    auto scale = params.is_mobile ? scale_w : math::min(scale_w, scale_h);

    auto w = math::cxpr::round_to_unsigned<u32>(scale * game_w);
    auto h = math::cxpr::round_to_unsigned<u32>(scale * game_h);
    
    Vec2Du32 window_dims = { w, h };        

#ifdef APP_ROTATE_90

    return window::create(mv::window, game::APP_TITLE, window_dims, game_dims, mv::GAME_ROTATE);

#else

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


static img::ImageView make_window_view()
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

    //read_controller_state(mv::em_controller, prev.gamepads[0], input.gamepads[0]);
    // map touch to gamepad

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

}


static void print_credits()
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

    emscripten_set_main_loop(main_loop, 0, 1);

    mv::run_state = RunState::Run;
    
    main_close();

    return EXIT_SUCCESS;
}


#include "../main_o/main_o_sdl3.cpp"