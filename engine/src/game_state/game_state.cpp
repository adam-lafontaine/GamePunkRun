#include "game_state.hpp"
#include "../../../libs/stb_libs/qsprintf.hpp"
#include "../../../libs/util/numeric.hpp"
#include "../../../libs/datetime/datetime.hpp"

#include <thread>

#define GAME_PUNK 1


/* helpers */

namespace game_state
{
    namespace img = image;
    namespace num = numeric;


    static void show_vec(cstr label, Vec2Du32 vec)
    {
        ImGui::Text("%s: {%u, %u}", label, vec.x, vec.y);
    }


    static void show_vec(cstr label, Vec2Df32 vec)
    {
        ImGui::Text("%s: {%f, %f}", label, vec.x, vec.y);
    }


    static void show_vec(cstr label, Vec2D<i8> vec)
    {
        ImGui::Text("%s: {%d, %d}", label, (int)vec.x, (int)vec.y);
    }


    static void show_vec(cstr label, Vec2D<i16> vec)
    {
        ImGui::Text("%s: {%d, %d}", label, (int)vec.x, (int)vec.y);
    }


    static void show_time_sec(cstr label, f32 sec)
    {
        constexpr f32 NANO =  1.0f / 1'000'000'000;
        constexpr f32 MICRO = 1.0f / 1'000'000;
        constexpr f32 MILLI = 1.0f / 1'000;

        constexpr auto nano = "ns";
        constexpr auto micro = "us";
        constexpr auto milli = "ms";
        constexpr auto s = "sec";

        cstr unit = s;

        if (sec < NANO)
        {
            unit = nano;
            sec *= 1'000'000'000;
        }
        else if (sec < MICRO)
        {
            unit = micro;
            sec *= 1'000'000;
        }
        else if (sec < MILLI)
        {
            unit = milli;
            sec *= 1'000;
        }

        ImGui::Text("%s: %f %s", label, sec, unit);
    }


    static void show_rect(cstr label, Rect2Du32 r)
    {
        ImGui::Text("%s: {%u, %u, %u, %u}", label, r.x_begin, r.y_begin, r.x_end, r.y_end);
    }


    static void thread_sleep_ms(int ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }


    static ImVec4 to_im_color(img::Pixel p)
    {
        return ImVec4(p.red / 255.0f, p.green / 255.0f, p.blue / 255.0f, p.alpha / 255.0f );
    }
}


/* plotting */

namespace game_state
{
    class PlotProps
    {
    private:
        static constexpr int N = 256;

    public:
        cstr label = " ";

        cstr units = " ";

        f32 plot_data[N] = { 0 };
        int data_count = N;
        int data_offset = 0;
        int data_stride = sizeof(f32);
        f32 plot_min = 0.0f;
        f32 plot_max = 0.0f;
        ImVec2 plot_size = ImVec2(0, 100.0f);

        bool enabled = false;
        bool started = false;
        bool minmax = false;

        int count = 0;
        f32 total = 0.0f;
        f32 avg = 0.0f;

        void add_data(f32 val)
        {            
            if (count == N)
            {
                total -= plot_data[data_offset];
            }

            total += val;

            plot_data[data_offset] = val;
            data_offset = (data_offset + 1) & (N - 1);

            count = num::min(count + 1, N);

            avg = total / count;            

            if (!minmax)
            {                
                plot_min = plot_max = val;

                minmax = true;
            }

            plot_min = num::min(val, plot_min);
            plot_max = num::max(val, plot_max);
        }


        void reset()
        {
            enabled = false;
            started = false;
            minmax = false;
            count = 0;
            total = 0.0f;
            avg = 0.0f;
        }

    };


    static void show_plot(PlotProps& props, cstr label)
    {
        char label_text[32] = { 0 };
        stb::qsnprintf(label_text, 32, "avg: %f##%s", props.avg, label);

        char overlay[32] = { 0 };

        ImGui::Text("min: %f %s, max: %f %s", props.plot_min, props.units, props.plot_max, props.units);
        ImGui::PlotLines(label_text, 
            props.plot_data, 
            props.data_count, 
            props.data_offset, 
            overlay,
            props.plot_min, props.plot_max, 
            props.plot_size, 
            props.data_stride);
    }


    static void start_proc(PlotProps& props)
    {
        props.units = "ms";

        auto copy_loop = [&]()
        {
            /*auto& data = game::get_data(mbt_state);
            auto r = img::make_rect(data.screen_dims.x, data.screen_dims.y);

            Stopwatch sw;
            while (game_running && props.enabled)
            {
                sw.start();
                //game::proc_copy(data.mbt_mat, r, r);
                props.add_data((f32)sw.get_time_milli());

                thread_sleep_ms(10);
            }*/
        };

        std::thread th(copy_loop);
        th.detach();        
    }
}


#if GAME_PUNK

#include "game_punk_state.cpp"

#else


namespace game_state
{    
    img::ImageView game_view;

    bool init(Vec2Du32& screen_dimensions)
    {        
        screen_dimensions = { 400, 400 };

        return true;
    }


    bool set_screen_memory(img::ImageView screen)
    {
        game_view = screen;
        return true;
    }


    void update(input::Input const& input)
    {
        img::fill(game_view, img::to_pixel(50, 150, 50));
    }


    void reset()
    {
        
    }


    void close()
    {
        
    }


    void show_game_state()
    {

    }
}


#include "../../../libs/alloc_type/alloc_type.cpp"
#include "../../../libs/span/span.cpp"
#include "../../../libs/image/image.cpp"
#include "../../../libs/stb_libs/stb_libs.cpp"
#include "../../../libs/ascii_image/ascii_image.cpp"

#ifdef USE_SDL2

#include "../../../libs/sdl2/sdl_input.cpp"
#include "../../../libs/sdl2/sdl_window.cpp"

#ifndef NO_AUDIO
#include "../../../libs/sdl2/sdl_audio.cpp"
#endif

#else // SDL3

#include "../../../libs/sdl3/sdl_input.cpp"
#include "../../../libs/sdl3/sdl_window.cpp"

#ifndef NO_AUDIO
#include "../../../libs/sdl3/sdl_audio.cpp"
#endif

#endif

#endif