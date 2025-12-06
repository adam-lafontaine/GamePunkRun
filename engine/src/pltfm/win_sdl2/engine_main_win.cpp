#include "../../io_test/app/app.hpp"

#include "../imgui_sdl2_dx11/imgui_include.hpp"
#include "../../ui/ui.hpp"
#include "../../game_state/game_state.hpp"
#include "../../../../libs/datetime/datetime.hpp"

#include <thread>

#define app_assert(condition) SDL_assert(condition)
#define app_log(...) SDL_Log(__VA_ARGS__)


namespace img = image;
namespace iot = game_io_test;
namespace gs = game_state;

using Stopwatch = datetime::StopwatchNS;


enum class RunState : int
{
    Begin,
    Run,
    End
};


/* main variables */

namespace mv
{
    ui_imgui::UIState ui_state{};
    RunState run_state = RunState::Begin;

    input::InputArray inputs;

    Stopwatch frame_sw;
    Stopwatch game_sw;
    EngineState state{};

    constexpr u32 N_TEXTURES = 2;
    dx11_imgui::TextureList<N_TEXTURES> textures;

    img::Image io_test_screen;
    iot::AppState io_test_state;    
    constexpr auto io_test_texture_id = dx11_imgui::to_texture_id(0);

    img::Image game_screen;
    constexpr auto game_texture_id = dx11_imgui::to_texture_id(1);
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

    auto ns = mv::frame_sw.get_time_nano();

    auto sleep_ns = TARGET_NS_PER_FRAME - ns;
    if (sleep_ns > 0.0)
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds((i64)(sleep_ns * fudge)));
    }
    
    mv::state.frame_nano = mv::frame_sw.get_time_nano();
    mv::frame_sw.start();
}


static void add_frame_time()
{
    auto ratio = (f32)(mv::state.game_nano / mv::state.frame_nano);
    mv::state.frame_times.add_time(ratio);
}


static void set_window_icon(SDL_Window* window)
{
#include "../../../res/icon_64.c"

    ui_imgui::set_window_icon(window, icon_64);
}


static void handle_imgui_sdl_event(void* event_ptr)
{
    auto event = (SDL_Event*)event_ptr;
    ui_imgui::handle_sdl_event(mv::ui_state, event);
}


static void process_user_input()
{
    input::record_input(mv::inputs, handle_imgui_sdl_event);
}


static void render_textures()
{
    auto& io_test_texture = mv::textures.get_dx_texture_ref(mv::io_test_texture_id);
    auto& game_texture = mv::textures.get_dx_texture_ref(mv::game_texture_id);

    dx11_imgui::render_texture(io_test_texture, mv::ui_state.dx_context);
    dx11_imgui::render_texture(game_texture, mv::ui_state.dx_context);
}


static void render_imgui_frame()
{
    ui_imgui::new_frame();
    ui_imgui::show_imgui_demo(mv::ui_state);

    auto t = mv::textures.get_im_texture_id(mv::io_test_texture_id);
    auto w = mv::io_test_screen.width;
    auto h = mv::io_test_screen.height;
    ui::io_test_window(t, w, h, mv::state);

    t = mv::textures.get_im_texture_id(mv::game_texture_id);
    w = mv::game_screen.width;
    h = mv::game_screen.height;
    ui::game_window("GAME", t, w, h, mv::state);

    ui::diagnostics_window();
    ui::input_frames_window(mv::state);
    ui::game_control_window(mv::state);

    ui_imgui::render(mv::ui_state);
}


static bool io_test_init()
{
    auto result = iot::init(mv::io_test_state);
    if (!result.success)
    {
        return false;
    }

    auto w = result.screen_dimensions.x;
    auto h = result.screen_dimensions.y;
    if (!w || !h)
    {
        return false;
    }

    if (!img::create_image(mv::io_test_screen, w, h))
    {
        return false;
    }

    auto& texture = mv::textures.get_dx_texture_ref(mv::io_test_texture_id);
    dx11_imgui::init_texture(mv::io_test_screen.data_, (int)w, (int)h, texture, mv::ui_state.dx_context);

    return iot::set_screen_memory(mv::io_test_state, img::make_view(mv::io_test_screen));
}


static bool game_state_init()
{
    Vec2Du32 dims = { 0 };
    if (!gs::init(dims))
    {
        return false;
    }

    auto w = dims.x;
    auto h = dims.y;
    if (!w ||! h)
    {
        return false;
    }

    if (!img::create_image(mv::game_screen, w, h))
    {
        return false;
    }

    auto& texture = mv::textures.get_dx_texture_ref(mv::game_texture_id);
    dx11_imgui::init_texture(mv::game_screen.data_, (int)w, (int)h, texture, mv::ui_state.dx_context);

    return gs::set_screen_memory(img::make_view(mv::game_screen));
}


static bool main_init()
{
#ifdef NDEBUG
    mv::ui_state.window_title = "EPC2 SDL2";
#else
    mv::ui_state.window_title = "EPC2 SDL2 (Debug)";
#endif
    
    mv::ui_state.window_width = 1400;
    mv::ui_state.window_height = 700;
    
    if (!ui_imgui::init(mv::ui_state))
    {
        return false;
    }

    set_window_icon(mv::ui_state.window);

    if (!input::init(mv::inputs))
    {
        return false;
    }

    mv::textures = dx11_imgui::create_textures<mv::N_TEXTURES>();

    if (!io_test_init())
    {
        return false;
    }

    if (!game_state_init())
    {
        return false;
    }

    return true;
}


static void main_close()
{
    iot::close(mv::io_test_state);
    img::destroy_image(mv::io_test_screen);

    ui_imgui::close(mv::ui_state);
}


/*static void game_loop()
{
    mv::game_sw.start();
    while(is_running())
    {
        process_user_input();
        auto& input = mv::inputs.curr();

        if (mv::state.cmd_reset_game)
        {
            gs::reset();
            mv::state.cmd_reset_game = 0;
        }

        if (mv::state.cmd_toggle_pause)
        {
            mv::state.cmd_toggle_pause = 0;
            mv::state.hard_pause = !mv::state.hard_pause;
        }

        if (!mv::state.hard_pause)
        {
            mv::game_sw.start();
            gs::update(input);
            mv::state.game_nano = mv::game_sw.get_time_nano();
        }

        mv::inputs.swap();

        cap_framerate();
        add_frame_time();
    }
}


static void main_loop()
{
    std::thread th(game_loop);

    while(is_running())
    {
        auto input_copy = mv::inputs.curr();

        iot::update(mv::io_test_state, input_copy); 

        render_textures();

        render_imgui_frame();

        if (mv::ui_state.cmd_end_program || input_copy.cmd_end_program)
        {
            end_program();
        }
    }

    th.join();
}*/


static void main_loop_seq()
{
    mv::game_sw.start();

    while(is_running())
    {
        process_user_input();
        auto& input = mv::inputs.curr();

        if (mv::state.cmd_reset_game)
        {
            gs::reset();
            mv::state.cmd_reset_game = 0;
        }

        if (mv::state.cmd_toggle_pause)
        {
            mv::state.cmd_toggle_pause = 0;
            mv::state.hard_pause = !mv::state.hard_pause;
        }

        if (!mv::state.hard_pause)
        {
            mv::game_sw.start();
            gs::update(input);
            mv::state.game_nano = mv::game_sw.get_time_nano();
        }

        if (mv::ui_state.cmd_end_program || input.cmd_end_program)
        {
            end_program();
        }        

        iot::update(mv::io_test_state, input); 

        render_textures();

        render_imgui_frame();

        mv::inputs.swap();

        cap_framerate();
        add_frame_time();
    }
}


int main()
{
    if (!main_init())
    {
        return 1;
    }

    mv::run_state = RunState::Run;

    main_loop_seq();

    main_close();

    return 0;
}

#include "../main_o/main_o_sdl2.cpp"