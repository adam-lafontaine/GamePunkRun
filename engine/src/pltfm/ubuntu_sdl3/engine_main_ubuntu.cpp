#ifndef NDEBUG
#define app_assert(condition) SDL_assert(condition)
#define app_log(...) SDL_Log(__VA_ARGS__)
#define app_crash(message) SDL_assert(false && message)
#else
#define app_assert(condition)
#define app_log(...)
#define app_crash(message)
#endif

#include "../../io_test/app/app.hpp"

#include "../imgui_sdl3_ogl3/imgui_include.hpp"
#include "../../ui/ui.hpp"
#include "../../game_state/game_state.hpp"
#include "../../../../libs/datetime/datetime.hpp"


namespace img = image;
namespace iot = game_io_test;
namespace gs = game_state;
namespace dt = datetime;

using Stopwatch = dt::Stopwatch;


enum class RunState : int
{
    Begin,
    Run,
    End
};


/* main variables */

namespace mv
{
#ifdef NDEBUG

    constexpr auto APP_TITLE = "Engine SDL3";

#else

    constexpr auto APP_TITLE = "Engine SDL3 (Debug)";

#endif

    ui_imgui::UIState ui_state{};
    RunState run_state = RunState::Begin;

    input::InputArray inputs;

    Stopwatch frame_sw;
    Stopwatch game_sw;
    EngineState state{};

    constexpr u32 N_TEXTURES = 2;
    ogl_imgui::TextureList<N_TEXTURES> textures;

    img::Image io_test_screen;
    iot::AppState io_test_state;    
    constexpr ogl_imgui::TextureId io_test_texture_id = ogl_imgui::to_texture_id(0);

    // display zoom 2x, rotated
    constexpr u32 GAME_ZOOM_SCALE = 2;
    img::Buffer32 game_screen_buffer;
    img::ImageView game_screen_1x;
    img::ImageView game_screen_scale;
    img::ImageView game_screen_scale_rotate;
    constexpr ogl_imgui::TextureId game_texture_id = ogl_imgui::to_texture_id(1);
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

    mv::state.frame_nano = mv::frame_sw.get_time_nano_f64();
    mv::frame_sw.start();
}


static void add_frame_time()
{
    auto time = (f32)mv::state.game_milli;
    mv::state.frame_times.add_time(time);
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
    auto& io_test_texture = mv::textures.get_gl_texture_ref(mv::io_test_texture_id);
    auto& game_texture = mv::textures.get_gl_texture_ref(mv::game_texture_id);

    ogl_imgui::render_texture(io_test_texture);
    ogl_imgui::render_texture(game_texture);
}


static void render_imgui_frame()
{
    ui_imgui::new_frame();
    ui_imgui::show_imgui_demo(mv::ui_state);

    auto t = mv::textures.get_im_texture_id(mv::io_test_texture_id);
    auto w = mv::io_test_screen.width;
    auto h = mv::io_test_screen.height;
    mv::state.iot_active = ui::texture_window("Input", t, w, h, mv::state.iot_display_scale);

    t = mv::textures.get_im_texture_id(mv::game_texture_id);
    w = mv::game_screen_scale_rotate.width;
    h = mv::game_screen_scale_rotate.height;
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

    auto& texture = mv::textures.get_gl_texture_ref(mv::io_test_texture_id);
    ogl_imgui::init_texture(mv::io_test_screen.data_, (int)w, (int)h, texture);

    return iot::set_screen_memory(mv::io_test_state, img::make_view(mv::io_test_screen));
}


static bool game_state_init()
{
    Vec2Du32 dims = { 0 };
    if (!gs::init(dims))
    {
        app_log("*** game init() failed ***\n");
        return false;
    }

    auto w = dims.x;
    auto h = dims.y;
    if (!w ||! h)
    {
        app_log("*** bad game dimensions ***\n");
        return false;
    }

    auto ws = w * mv::GAME_ZOOM_SCALE;
    auto hs = h * mv::GAME_ZOOM_SCALE;

    auto wsr = hs;
    auto hsr = ws;

    auto count = w * h + ws * hs + wsr * hsr;

    mv::game_screen_buffer = img::create_buffer32(count, "game screens");
    if (!mv::game_screen_buffer.ok)
    {
        return false;
    }

    mv::game_screen_1x = img::make_view(w, h, mv::game_screen_buffer);
    mv::game_screen_scale = img::make_view(ws, hs, mv::game_screen_buffer);
    mv::game_screen_scale_rotate = img::make_view(wsr, hsr, mv::game_screen_buffer);

    auto data = mv::game_screen_scale_rotate.matrix_data_;

    auto& texture = mv::textures.get_gl_texture_ref(mv::game_texture_id);
    ogl_imgui::init_texture(data, (int)wsr, (int)hsr, texture);

    auto res = gs::set_screen_memory(mv::game_screen_1x);

    return res;
}


static void game_state_update(input::Input& input)
{
    mv::game_sw.start();

    gs::update(input);

    mv::state.game_milli = mv::game_sw.get_time_milli_f64();
    
    img::scale_up(mv::game_screen_1x, mv::game_screen_scale, mv::GAME_ZOOM_SCALE);
    img::rotate_270(mv::game_screen_scale, mv::game_screen_scale_rotate);
}


static bool main_init()
{    
    mv::ui_state.window_title = mv::APP_TITLE;    

    mv::ui_state.window_width = 1400;
    mv::ui_state.window_height = 950;
    
    if (!ui_imgui::init(mv::ui_state))
    {
        return false;
    }

    set_window_icon(mv::ui_state.window);

    if (!input::init(mv::inputs))
    {
        return false;
    }

    mv::textures = ogl_imgui::create_textures<mv::N_TEXTURES>();

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


static void main_loop_seq()
{
    mv::frame_sw.start();

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
            game_state_update(input);
        }

        if (mv::ui_state.cmd_end_program || input.cmd_end_program)
        {
            end_program();
        }

        if (mv::state.iot_active)
        {
            iot::update(mv::io_test_state, input);
        }

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

#include "../main_o/main_o_sdl3.cpp"