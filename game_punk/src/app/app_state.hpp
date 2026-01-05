#pragma once


/* background state */

namespace game_punk
{
    class BackgroundState
    {
    public:

        SkyAnimation sky;

        BackgroundAnimation bg_1;
        BackgroundAnimation bg_2;
    };


    static void reset_background_state(BackgroundState& bg)
    {   
        reset_sky_animation(bg.sky);

        reset_background_animation(bg.bg_1);
        reset_background_animation(bg.bg_2);
        bg.bg_1.speed_shift = 1;
        bg.bg_2.speed_shift = 0;
    }


    static void count_background_state(BackgroundState& bg, MemoryCounts& counts)
    {  
        count_sky_animation(bg.sky, counts);
        
        count_background_animation(bg.bg_1, counts);
        count_background_animation(bg.bg_2, counts);
    }


    static bool create_background_state(BackgroundState& bg_state, Memory& memory)
    {
        bool ok = true;

        ok &= create_sky_animation(bg_state.sky, memory);

        ok &= create_background_animation(bg_state.bg_1, memory);
        ok &= create_background_animation(bg_state.bg_2, memory);

        return ok;
    }


}


/* spritesheet state */

namespace game_punk
{
    class SpritesheetState
    {
    public:        

        SpritesheetView punk_run;
        SpritesheetView punk_idle;
    };


    static void count_spritesheet_state(SpritesheetState& ss_state, MemoryCounts& counts)
    {
        using Punk = bt::Spriteset_Punk;

        constexpr Punk list;
        constexpr auto run = Punk::Items::Punk_run;
        constexpr auto idle = Punk::Items::Punk_idle;

        count_view(ss_state.punk_run, counts, bt::item_at(list, run));
        count_view(ss_state.punk_idle, counts, bt::item_at(list, idle));
    }


    static bool create_spritesheet_state(SpritesheetState& ss_state, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(ss_state.punk_run, memory);
        ok &= create_view(ss_state.punk_idle, memory);

        return ok;
    }
}


/* tile state */

namespace game_punk
{
    class TileState
    {
    public:
        TileView floor_a;
        TileView floor_b;
    };


    static void count_tile_state(TileState& tiles, MemoryCounts& counts)
    {
        using Ex = bt::Tileset_ex_zone;

        constexpr Ex list;
        constexpr auto f2 = Ex::Items::floor_02;
        constexpr auto f3 = Ex::Items::floor_03;

        count_view(tiles.floor_a, counts, bt::item_at(list, f2));
        count_view(tiles.floor_b, counts, bt::item_at(list, f3));
    }


    static bool create_tile_state(TileState& tiles, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(tiles.floor_a, memory);
        ok &= create_view(tiles.floor_b, memory);

        return ok;
    }
}


/* ui state */

namespace game_punk
{
    class UIState
    {
    public:
    
        static constexpr u32 CTS = cxpr::color_table_size<bt::UIset_Font>();

        struct
        {
            ImageView title;            
            
            SpritesheetView font; // FontView
            SpritesheetView icons; // IconView

            p32 colors[CTS];
        } data;

        MemoryStack<p32> pixels;

        u8 font_color_id;

        struct 
        {
            b8 is_on;
            GameTick64 end_tick;

        } temp_icon;        
    };


    static void count_ui_state(UIState& ui, MemoryCounts& counts)
    {
        using Title = bt::UIset_Title;
        using Font = bt::UIset_Font;
        using Icons = bt::UIset_Icons;

        constexpr Title title;
        auto t_info = bt::item_at(title, Title::Items::title_main);
        count_view(ui.data.title, counts, t_info.width, t_info.height);        
        
        constexpr Font font;
        u32 n_chars = 10 + 26 * 2; // 0-9, A-Z x 2
        count_view(ui.data.font, counts, bt::item_at(font, Font::Items::font), n_chars);

        constexpr Icons icons;
        count_view(ui.data.icons, counts, bt::item_at(icons, Icons::Items::icons));

        auto length = cxpr::GAME_CAMERA_WIDTH_PX * cxpr::GAME_CAMERA_HEIGHT_PX;
        count_stack(ui.pixels, counts, length);
    }


    static bool create_ui_state(UIState& ui, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(ui.data.title, memory);
        ok &= create_view(ui.data.font, memory);
        ok &= create_view(ui.data.icons, memory);
        ok &= create_stack(ui.pixels, memory);

        return ok;
    }


    static void reset_ui_state(UIState& ui)
    {
        ui.temp_icon.is_on = 0;
        ui.temp_icon.end_tick = GameTick64::make(1);
    }


    static void begin_ui_frame(UIState& ui)
    {
        reset_stack(ui.pixels);
        span::fill(to_span(ui.pixels), COLOR_TRANSPARENT);
    }


    static bool set_ui_color(UIState& ui, u8 color_id)
    {
        bool ok = color_id < (ui.CTS - 1);
        app_assert(ok && "*** Invalid color id ***");

        /*auto color = ui.data.colors[color_id];
        bt::filter_update(to_image_view(ui.data.font), color, COLOR_TRANSPARENT);

        // temp icon color
        bt::filter_update(to_image_view(ui.data.icons), color, COLOR_BLACK);
        ui.font_color_id = color_id;*/

        return ok;
    }


    static SpriteView get_ui_alpha_num(UIState& ui, u32 set_id, char c)
    {
        auto& font = ui.data.font;
        auto dims = font.bitmap_dims;
        auto length = dims.proc.width * dims.proc.height;

        auto data = font.data;
        auto set = (set_id % 2) * 26;
        int id = 0;

        if ('0' <= c && c <= '9')
        {
            id = (c - '0');
            data = font.data + length * id;
        }
        else if ('A' <= c && c <= 'Z')
        {
            id = 10 + set + (c - 'A');
            data = font.data + length * id;
        }
        else if ('a' <= c && c <= 'z')
        {
            id = 10 + set + (c - 'a');
            data = font.data + length * id;
        }
        else
        {
            data = 0;
        }

        SpriteView view;
        view.dims = dims;
        view.data = data;

        return view;
    }


    static SpriteView get_ui_icon(UIState& ui, Randomf32& rng, GameTick64 game_tick)
    {
        auto& icons = ui.data.icons;
        auto dims = icons.bitmap_dims;
        auto width = dims.proc.width;
        auto height = dims.proc.height;
        auto length = width * height;

        auto id = 29;

        SpriteView view;
        view.dims = dims;
        view.data = push_elements(ui.pixels, length);

        auto dst = to_image_view(view);

        auto do_icon = [&](u8 color_id)
        {
            set_ui_color(ui, color_id);
            auto src = img::make_view(width, height, icons.data); // Frame
            img::copy_if_alpha(src, dst);

            src.matrix_data_ += id * length; // Icon
            img::copy_if_alpha(src, dst);
        };

        auto& icon = ui.temp_icon;

        if (game_tick >= icon.end_tick)
        {
            icon.is_on = !icon.is_on;

            auto delta = icon.is_on ? TickQty32::random(rng, 3, 40) : TickQty32::random(rng, 2, 10);
            icon.end_tick = game_tick + delta;
        }

        if (icon.is_on)
        {
            do_icon(20);
        }
        else
        {
            do_icon(7);
        }

        return view;
    }
}
