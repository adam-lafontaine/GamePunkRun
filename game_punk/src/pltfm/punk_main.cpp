#include "../../../libs/io/window.hpp"
#include "../../../libs/io/input/input.hpp"
#include "../../../libs/io/message.hpp"
#include "../../../libs/datetime/datetime.hpp"

#include "../app/app.hpp"


namespace img = image;
namespace game = game_punk;
namespace dt = datetime;


constexpr f64 NANO = 1'000'000'000;
constexpr f64 MICRO = 1'000'000;

constexpr f64 TARGET_FRAMERATE_HZ = 60.0;
constexpr f64 TARGET_NS_PER_FRAME = NANO / TARGET_FRAMERATE_HZ;
constexpr f64 TARGET_MS_PER_FRAME = TARGET_NS_PER_FRAME / MICRO;


// use game dimensions if not full screen
constexpr u32 WINDOW_WIDTH = 0;
constexpr u32 WINDOW_HEIGHT = 0;


using Stopwatch = dt::StopwatchNS;


enum class RunState : int
{
    Begin,
    Run,
    End
};


/* main variables */

namespace mv
{
    RunState run_state = RunState::Begin;

    window::Window window;
    input::InputArray input;

    game::AppState app_state;
    Stopwatch frame_sw;    

    constexpr int MAIN_ERROR = 1;
    constexpr int MAIN_OK = 0;

#ifdef APP_ROTATE_90
    constexpr window::Rotate GAME_ROTATE = window::Rotate::CounterClockwise_90;
#endif
}




void end_program()
{
    mv::run_state = RunState::End;
}


static bool is_running()
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


static bool window_create(Vec2Du32 game_dims)
{ 

#ifdef APP_ROTATE_90

    // rotated
    auto w = game_dims.y;
    auto h = game_dims.x;

    Vec2Du32 window_dims = {
        w > WINDOW_WIDTH ? w : WINDOW_WIDTH,
        h > WINDOW_HEIGHT ? h : WINDOW_HEIGHT
    };

    return window::create(mv::window, game::APP_TITLE, window_dims, game_dims, mv::GAME_ROTATE);

#else
    
    auto w = game_dims.x;
    auto h = game_dims.y;

    Vec2Du32 window_dims = {
        w > WINDOW_WIDTH ? w : WINDOW_WIDTH,
        h > WINDOW_HEIGHT ? h : WINDOW_HEIGHT
    };

    return window::create(mv::window, game::APP_TITLE, window_dims, game_dims);

#endif
}


static bool window_create_fullscreen(Vec2Du32 game_dims)
{
#ifdef APP_ROTATE_90
    return window::create_fullscreen(mv::window, game::APP_TITLE, game_dims, mv::GAME_ROTATE);
#else
    return window::create_fullscreen(mv::window, game::APP_TITLE, game_dims);
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


bool create_window(Vec2Du32 game_dims)
{
#ifndef APP_FULLSCREEN

    if (!window_create(game_dims))
    {
        return false;
    }

#else

    if (!window_create_fullscreen(game_dims))
    {
        return false;
    }

#endif

#include "../../res/icon/icon_64.cpp"
    window::Icon64 icon{};

    static_assert(sizeof(icon_64.pixel_data) >= icon.min_data_size);
    assert(icon_64.width == icon.width);
    assert(icon_64.height == icon.height);

    icon.pixel_data = (u8*)icon_64.pixel_data;

    window::set_window_icon(mv::window, icon);

    return true;
}


img::ImageView make_window_view()
{
    static_assert(window::PIXEL_SIZE == sizeof(img::Pixel));

    auto w = mv::window.width_px;
    auto h = mv::window.height_px;
    auto data = (img::Pixel*)mv::window.pixel_buffer;

    return img::make_view(w, h, data);
}


static bool main_init()
{  
    if (!window::init())
    {
        return false;
    }

    if (!input::init(mv::input))
    {
        return false;
    }

    auto result = game::init(mv::app_state);
    if (!result.success)
    {
        msg::error_dialogue(game::decode_error(result.error));
        return false;
    }

    if (!create_window(result.app_dimensions))
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
    Stopwatch sw;
    sw.start();

    while(is_running())
    {
        input::record_input(mv::input);
        auto& input = mv::input.curr();

        if (input.cmd_end_program)
        {
            end_program();
        }

        game::update(mv::app_state, input);

        window_render(input.window_size_changed);

        mv::input.swap();
        cap_framerate();
    }
}


int main()
{
    if (!main_init())
    {
        main_close();
        return mv::MAIN_ERROR;
    }

    mv::run_state = RunState::Run;

    main_loop();

    main_close();

    return mv::MAIN_OK;
}