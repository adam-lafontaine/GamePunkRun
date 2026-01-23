#pragma once


/* ui state */

namespace game_punk
{
    class UIState
    {
    public:
    
        static constexpr u32 CTS = cxpr::color_table_size<bt::UIset_Font>();

        struct
        {            
            SpritesheetView font; // FontView
            SpritesheetView icons; // IconView

            p32 colors[CTS];
        } data;

        ImageView fullscreen_view;

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
        using Font = bt::UIset_Font;
        using Icons = bt::UIset_Icons;
        
        constexpr Font font;
        u32 n_chars = 10 + 26 * 2; // 0-9, A-Z x 2
        count_view(ui.data.font, counts, bt::item_at(font, Font::Items::font), n_chars);

        constexpr Icons icons;
        count_view(ui.data.icons, counts, bt::item_at(icons, Icons::Items::icons));

        auto dims = CAMERA_DIMS.proc;
        count_view(ui.fullscreen_view, counts, dims.width, dims.height);

        auto length = dims.width * dims.height;
        count_stack(ui.pixels, counts, length);
    }


    static bool create_ui_state(UIState& ui, Memory& memory)
    {
        bool ok = true;
        
        ok &= create_view(ui.data.font, memory);
        ok &= create_view(ui.data.icons, memory);
        ok &= create_view(ui.fullscreen_view, memory);
        ok &= create_stack(ui.pixels, memory);

        return ok;
    }


    static void reset_ui_state(UIState& ui)
    {
        ui.temp_icon.is_on = 0;
        ui.temp_icon.end_tick = GameTick64::make(1);
        reset_stack(ui.pixels);
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


/* player state */

namespace game_punk
{
    
    class PlayerState
    {
    public:
        SpriteID sprite;
        SpriteMode current_mode;
    };


    static void set_player_mode(PlayerState& player, SpriteTable sprites, SpriteMode mode)
    {
        auto& vel = sprites.velocity_px_at(player.sprite);
        auto& afn = sprites.animate_at(player.sprite);

        vel.x = mode == SpriteMode::Idle ? 0 : 2;

        player.current_mode = mode;
        afn = get_animate_fn(SpriteName::Punk, mode);
    }
}