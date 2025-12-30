#pragma once

#include "../../res/xbin/bin_table.hpp"
#include "../../../libs/util/stack_buffer.hpp"


/* asset constants */

namespace game_punk
{
    namespace bt = bin_table;
    namespace sb = stack_buffer;


namespace cxpr
{

    constexpr Vec2Du32 background_dimensions()
    {
        constexpr bt::Background_Bg1 list;

        Vec2Du32 dims;
        
        dims.x = list.items[0].width;
        dims.y = list.items[0].height;

        return dims;
    }

    // rotated
    constexpr u32 GAME_BACKGROUND_WIDTH_PX = background_dimensions().y;
    constexpr u32 GAME_BACKGROUND_HEIGHT_PX = background_dimensions().x;


    constexpr u32 find_4k_scale()
    {
        auto w = WIDTH_4K;
        u32 n = 2;
        while (w >= GAME_BACKGROUND_WIDTH_PX)
        {
            w = WIDTH_4K / n++;
        }        

        return n + 1;
    }

    constexpr u32 SCALE_4K = find_4k_scale();

    constexpr u32 GAME_CAMERA_WIDTH_PX = WIDTH_4K / SCALE_4K;
    constexpr u32 GAME_CAMERA_HEIGHT_PX = HEIGHT_4K / SCALE_4K;


    constexpr Vec2Du32 sky_overlay_dimensions()
    {
        constexpr bt::SkyOverlay_overlay list;

        Vec2Du32 dims;
        
        dims.x = list.items[0].width;
        dims.y = list.items[0].height;

        return dims;
    }


    constexpr u32 SKY_OVERLAY_WIDTH_PX = sky_overlay_dimensions().x;
    constexpr u32 SKY_OVERLAY_HEIGHT_PX = sky_overlay_dimensions().y;


    template <class T>
    constexpr u32 color_table_size()
    {
        return T::color_table.width;
    }


    constexpr u32 BACKGROUND_1_COUNT = bt::Background_Bg1::count;
    constexpr u32 BACKGROUND_2_COUNT = bt::Background_Bg2::count;
    constexpr u32 BACKGROUND_COUNT_MAX = math::cxpr::max(BACKGROUND_1_COUNT, BACKGROUND_2_COUNT);



} // cxpr    

}

/* helpers */

namespace game_punk
{
    template <typename T>
    static bool rect_intersect(Rect2D<T> const& a, Rect2D<T> const& b)
    {
        return 
            a.x_begin < b.x_end &&
            a.x_end > b.x_begin &&
            a.y_begin < b.y_end &&
            a.y_end > b.y_begin;
    }
    

    static Rect2Du32 clamp_rect(Rect2Di32 rect, Rect2Di32 out)
    {
        Rect2Du32 dr{};
        dr.x_begin = (u32)math::max(rect.x_begin, out.x_begin);
        dr.y_begin = (u32)math::max(rect.y_begin, out.y_begin);
        dr.x_end = (u32)math::min(rect.x_end, out.x_end);
        dr.y_end = (u32)math::min(rect.y_end, out.y_end);

        return dr;
    }


    static void add_pma(Span32 const& src, Span32 const& dst)
    {
        p32 ps;

        for (u32 i = 0; i < src.length; i++)
        {
            ps = src.data[i];
            auto& pd = dst.data[i];

            pd.red += ps.red;
            pd.green += ps.green;
            pd.blue += ps.blue;
        }
    }


    template <typename T>
    bool has_data(T const& item)
    {
        return !(!item.data);
    }


    bool has_data(ImageView const& view)
    {
        return !(!view.matrix_data_);
    }

}


/* span cxpr */

namespace game_punk
{
    template <u32 W, u32 H>
    class Span32_cxpr
    {
    public:
        static constexpr u32 length = W * H;
        p32* data = 0;

        constexpr Span32_cxpr(p32* pixels) { data = pixels; }
    };


    template <u32 W, u32 H>
    static inline Span32 to_span(Span32_cxpr<W, H> const& sp)
    {
        return span::make_view(sp.data, sp.length);
    }


    template <u32 W, u32 H>
    static inline void copy(Span32_cxpr<W, H> const& src, Span32_cxpr<W, H> const& dst)
    {
        span::copy(to_span(src), to_span(dst));
    }


    template <u32 W, u32 H>
    static inline Span32 sub_view(Span32_cxpr<W, H> const& src, u32 offset, u32 length)
    {
        return span::sub_view(to_span(src), offset, length);
    }
}


/* asset data */

namespace game_punk
{
    enum class AssetStatus : u8
    {
        None = 0,
        Loading,
        Success,

        FailLoad,
        FailRead
    };


    class AssetData
    {
    public:
        AssetStatus status = AssetStatus::None;

        cstr bin_file_path = 0;

        MemoryBuffer<u8> bytes;
    };


    static void destroy_asset_data(AssetData& gd)
    {
        mb::destroy_buffer(gd.bytes);
    }


    class LoadContext
    {
    public:
        ImageView dst;
        p32 color = COLOR_BLACK;
        u32 item_id = 0;
    };
    
    
    using OnAssetLoad = void (*)(Buffer8 const&, LoadContext const&);

    
    class LoadCommand
    {
    public:
        b8 is_active = 0;

        LoadContext ctx;

        OnAssetLoad on_load;
    };
    
    
    class LoadQueue
    {
    public:
        u32 capacity = 0;
        u32 size = 0;

        LoadCommand* commands = 0;
    };


    static void push_load(LoadQueue& q, LoadCommand cmd)
    {
        auto i = q.size;

        if (i < q.capacity && cmd.is_active)
        {
            q.commands[i] = cmd;
            q.size++;
        }        
    }


    static void load_all(AssetData const& src, LoadQueue& q)
    {
        for (u32 i = 0; i < q.size; i++)
        {
            auto& cmd = q.commands[i];            
            cmd.on_load(src.bytes, cmd.ctx);
        }

        q.size = 0;
    }


    template <class LIST>
    static void load_background_image(Buffer8 const& buffer, LoadContext const& ctx)
    {
        constexpr LIST list;

        auto item = static_cast<LIST::Items>(ctx.item_id);
        auto filter = list.read_alpha_filter_item(buffer, item);
        bt::alpha_filter_convert(filter, ctx.dst, ctx.color);
        filter.destroy();
    }


    template <class LIST>
    static void load_spritesheet_image(Buffer8 const& buffer, LoadContext const& ctx)
    {
        constexpr LIST list;

        auto table = list.read_table(buffer);
        auto item = static_cast<LIST::Items>(ctx.item_id);
        auto filter = list.read_table_filter_item(buffer, item);
        bt::color_table_convert(filter, table, ctx.dst);

        table.destroy();
        filter.destroy();
    }


    template <class LIST>
    static void load_tile_image(Buffer8 const& buffer, LoadContext const& ctx)
    {
        constexpr LIST list;

        auto table = list.read_table(buffer);
        auto item = static_cast<LIST::Items>(ctx.item_id);
        auto filter = list.read_table_filter_item(buffer, item);
        bt::color_table_convert(filter, table, ctx.dst);

        table.destroy();
        filter.destroy();
    }
}


/* random */

namespace game_punk
{
    class Randomf32
    {
    public:
        static constexpr u32 capacity = 256;

        f32 values[capacity];

        u8 b_cursor = 0;
        u8 r_cursor = 0;

    };


    static void reset_random(Randomf32& rng)
    {
        rng.b_cursor = 0;
        rng.r_cursor = 0;

        math::rand_init();

        for (u32 i = 0; i < rng.capacity; i++)
        {
            rng.values[i] = math::rand(0.0f, 1.0f);
        }
    }


    static void refresh_random(Randomf32& rng)
    {
        for (u8 i = rng.b_cursor; i < rng.r_cursor; i++)
        {
            rng.values[i] = math::rand(0.0f, 1.0f);
        }

        rng.b_cursor = rng.r_cursor;
    }


    u32 next_random_u32(Randomf32& rng, u32 min, u32 max)
    {
        auto val = rng.values[rng.r_cursor++];
        app_assert(rng.r_cursor != rng.b_cursor && "*** Frame RNG exceded ***");

        auto delta = val * (max - min) + 0.5f;

        return min + (u32)delta;
    }


    /*class PickRandom
    {
    public:
        u8 excluded[4] = { 0, 1, 2, 3 };
        u8 included[10 - 4] = { 4, 5, 6, 7, 8, 9 };

        u32 exc = 0;

        u8 pick(Randomf32& rng)
        {
            auto inc = next_random_u32(rng, 0, 5);
            auto val = included[inc];

            included[inc] = excluded[exc];
            excluded[exc] = val;
            exc = (exc + 1) & (4 - 1);

            return val;
        }
    };*/
}


/* integral types */

namespace game_punk
{
    class GameTick64
    {
    private:

        constexpr GameTick64(u64 v) { static_assert(sizeof(GameTick64) == 8); value_ = v; }

    public:

        u64 value_ = 0;

        //constexpr GameTick64(u64 v) { static_assert(sizeof(GameTick64) == 8); value_ = v; }

        //GameTick64(u64 v) = delete;

        GameTick64() = delete;

        static constexpr GameTick64 make(u64 v) { return GameTick64(v); }

        static constexpr GameTick64 zero() { return make(0u); }

        static constexpr GameTick64 last() { return make((u64)0 - (u64)1); }    
        

        GameTick64& operator ++ () { ++value_; return *this; }


        bool operator == (GameTick64 other) const { return value_ == other.value_; }

        bool operator >= (GameTick64 other) const { return value_ >= other.value_; }
        
    };


    class TickQty32
    {
    public:
        
        u32 value_ = 0;
    

        TickQty32() { value_ = 0; }

        constexpr TickQty32(u32 v) { value_ = v; }


        static constexpr TickQty32 make(u32 v) { return TickQty32(v); }

        static constexpr TickQty32 zero() { return TickQty32(0u); }


        TickQty32& operator = (GameTick64 other) { value_ = other.value_; return *this; }

        //TickQty32& operator += (TickQty32 other) { value_ += other.value_; return *this; }

        bool operator == (TickQty32 other) const { return value_ == other.value_; }

        bool operator < (TickQty32 other) const { return value_ < other.value_; }

        bool operator <= (TickQty32 other) const { return value_ <= other.value_; }

        bool operator >= (TickQty32 other) const { return value_ >= other.value_; }


        static TickQty32 random(Randomf32& rng, u32 min, u32 max) { return TickQty32(next_random_u32(rng, min, max)); }
    };


    GameTick64 operator + (GameTick64 lhs, TickQty32 rhs) { return GameTick64::make(lhs.value_ + rhs.value_); }

    TickQty32 operator - (GameTick64 lhs, TickQty32 rhs) { return lhs.value_ - rhs.value_; }

    TickQty32 operator % (GameTick64 lhs, TickQty32 rhs) { return lhs.value_ % rhs.value_; }


    bool operator <= (TickQty32 lhs, GameTick64 rhs) { return lhs.value_ <= rhs.value_; }

    //bool operator >= (TickQty lhs, u32 rhs) { return lhs.value_ >= (u64)rhs; }

    TickQty32 operator + (TickQty32 lhs, TickQty32 rhs) { return lhs.value_ + rhs.value_; }

    TickQty32 operator - (TickQty32 lhs, TickQty32 rhs) { return lhs.value_ - rhs.value_; }


    template <typename uT, u64 N>
    class uN2
    {
    public:
        static constexpr uT COUNT = (uT)N;
        static constexpr uT MAX_VALUE = COUNT - 1;

    private:
        static uT n2_add(uT a, uT b) { return (a + b) & MAX_VALUE; }
        static uT n2_sub(uT a, uT b) { return (a - b) & MAX_VALUE; }

    public:
        uT value_ = 0;


        constexpr uN2()
        {
            static_assert(math::cxpr::is_unsigned<uT>());
            static_assert(math::cxpr::is_power_of_2(N));
        }


        void reset() { value_ = 0; }


        uN2& operator ++ () { value_ = n2_add(value_, 1); return *this; }
    };
    
}


/* circular buffer */

namespace game_punk
{
    template <typename T, u32 COUNT>
    class RingStackBuffer
    {
    public:
        static constexpr u32 count = COUNT;

        T data[COUNT];

        uN2<u32, COUNT> cursor;

        T& front() { return data[cursor.value_]; }

        void next() { ++cursor; }        
    };


    template <typename T, u32 COUNT>
    class RandomStackBuffer
    {
    public:
        static constexpr u32 capacity = COUNT;

        u32 size = 0;

        T data[COUNT];

        u32 id = 0;

        T& get(Randomf32& rng) { id = next_random_u32(rng, 0, size - 1); return data[id]; }

        void set(T value) { data[id] = value; }
    };
}


/* orientation context */

namespace game_punk
{
    enum class DimCtx
    {
        Proc,
        Game
    };
    
    
    class ContextDims
    {
    public:
        union
        {
            struct { u32 width; u32 height; } proc;

            struct { u32 height; u32 width; } game;

            u64 any = 0;
        };

        constexpr ContextDims(u32 w, u32 h, DimCtx ctx)
        {
            if (ctx == DimCtx::Proc)
            {
                proc.width = w;
                proc.height = h;
            }
            else
            {
                game.width = w;
                game.height = h;
            }
        }


        ContextDims() { any = 0; }
    };


    template <typename T>
    class ContextVec2D
    {
    public:
        union
        {
            Vec2D<T> proc;

            struct { T y; T x; } game;
        };


        ContextVec2D() { proc.x = 0; proc.y = 0; }


        ContextVec2D(T x, T y, DimCtx ctx)
        {
            if (ctx == DimCtx::Proc)
            {
                proc.x = x;
                proc.y = y;
            }
            else
            {
                game.x = x;
                game.y = y;
            }
        }


        ContextVec2D(Vec2D<T> const& vec, DimCtx ctx)
        {
            ContextVec2D(vec.x, vec.y, ctx);
        }
    };


    class GamePosition
    {
    public:
        union
        {
            Point2Du64 proc;

            struct { u64 y; u64 x; } game;
        };


        GamePosition(u64 x, u64 y, DimCtx ctx) 
        {
            if (ctx == DimCtx::Proc)
            {
                proc.x = x;
                proc.y = y;
            }
            else
            {
                game.x = x;
                game.y = y;
            }
        }


        static GamePosition zero() { return GamePosition(0, 0, DimCtx::Proc); }
    };


    class BackgroundPosition
    {
    public:    
        union
        {
            Point2Di32 proc;

            struct { i32 y; i32 x; } game;
        };


        BackgroundPosition(i32 x, i32 y, DimCtx ctx)
        {
            if (ctx == DimCtx::Proc)
            {
                proc.x = x;
                proc.y = y;
            }
            else
            {
                game.x = x;
                game.y = y;
            }
        }
    };


    class ScreenPosition
    {
    public:
        union
        {
            Point2Di32 proc;

            struct { i32 y; i32 x; } game;
        };


        ScreenPosition(i32 x, i32 y, DimCtx ctx)
        {
            if (ctx == DimCtx::Proc)
            {
                proc.x = x;
                proc.y = y;
            }
            else
            {
                game.x = x;
                game.y = y;
            }
        }
    };    


    Point2Di32 delta_pos_px(BackgroundPosition a, BackgroundPosition b)
    {
        Point2Di32 p;
        p.x = a.proc.x - b.proc.x;
        p.y = a.proc.y - b.proc.y;

        return p;
    }


    class ContextRect
    {
    public:
        union
        {
            struct { u32 x_begin; u32 x_end; u32 y_begin; u32 y_end; } proc;

            struct { u32 y_begin; u32 y_end; u32 x_begin; u32 x_end; } game;
        };
    };


    static ContextRect make_rect(ContextDims dims)
    {
        ContextRect rect;

        rect.proc.x_begin = 0;
        rect.proc.y_begin = 0;
        rect.proc.x_end = dims.proc.width;
        rect.proc.y_end = dims.proc.height;

        return rect;
    }


    constexpr auto BACKGROUND_DIMS = ContextDims(cxpr::GAME_BACKGROUND_WIDTH_PX, cxpr::GAME_BACKGROUND_HEIGHT_PX, DimCtx::Game);

    constexpr auto CAMERA_DIMS = ContextDims(cxpr::GAME_CAMERA_WIDTH_PX, cxpr::GAME_CAMERA_HEIGHT_PX, DimCtx::Game);


}


#include "image_view.hpp"
#include "animation.hpp"


/* background images */

namespace game_punk
{
    class BackgroundState
    {
    public:

        SkyAnimation sky;

        BackgroundAnimationFast bgf_1;
        BackgroundAnimationFast bgf_2;

        //BackgroundAnimation bg_1;
        //BackgroundAnimation bg_2;
    };


    static void reset_background_state(BackgroundState& bg)
    {   
        reset_sky_animation(bg.sky);

        /*reset_background_animation(bg.bg_1);
        reset_background_animation(bg.bg_2);
        bg.bg_2.speed_shift = 1;*/

        reset_background_animation(bg.bgf_1);
        reset_background_animation(bg.bgf_2);
        bg.bgf_2.speed_shift = 1;
    }


    static void count_background_state(BackgroundState& bg, MemoryCounts& counts)
    {  
        count_sky_animation(bg.sky, counts);
        
        //count_background_animation(bg.bg_1, counts);
        //count_background_animation(bg.bg_2, counts);

        count_background_animation(bg.bgf_1, counts, bt::Background_Bg1::count);
        count_background_animation(bg.bgf_2, counts, bt::Background_Bg2::count);
    }


    static bool create_background_state(BackgroundState& bg_state, Memory& memory)
    {
        bool ok = true;

        ok &= create_sky_animation(bg_state.sky, memory);

        //ok &= create_background_animation(bg_state.bg_1, memory);
        //ok &= create_background_animation(bg_state.bg_2, memory);

        ok &= create_background_animation(bg_state.bgf_1, memory);
        ok &= create_background_animation(bg_state.bgf_2, memory);

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
        bool ok = color_id < ui.CTS;
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


/* screen camera */

namespace game_punk
{ 
    
    class ScreenCamera
    {
    public:

        static constexpr auto dims = CAMERA_DIMS;

        p32* pixels;

        BackgroundPosition bg_pos;
    };


    static bool init_screen_camera(ScreenCamera& camera, ImageView screen)
    {
        bool ok = true;

        auto dims = camera.dims.proc;

        ok &= has_data(screen);
        ok &= screen.width == dims.width;
        ok &= screen.height == dims.height;

        if (ok)
        {
            camera.pixels = screen.matrix_data_;
        }

        return ok;
    }


    static void reset_screen_camera(ScreenCamera& camera)
    {
        camera.bg_pos = BackgroundPosition(10, 16, DimCtx::Game);
    }


    static void move_camera(ScreenCamera& camera, Vec2Di8 delta_px)
    {
        auto max_dims = BACKGROUND_DIMS.game;
        auto cam_dims = CAMERA_DIMS.game;
        auto& pos = camera.bg_pos.game;

        auto x_max = (i32)(max_dims.width - cam_dims.width);
        auto y_max = (i32)(max_dims.height - cam_dims.height);        

        auto pos_x = (i32)pos.x;
        auto pos_y = (i32)pos.y;

        pos_x += delta_px.x;
        pos_y += delta_px.y;

        pos.x = (u32)math::cxpr::clamp(pos_x, 0, x_max);
        pos.y = (u32)math::cxpr::clamp(pos_y, 0, y_max);
    }


    static ImageView to_image_view(ScreenCamera const& camera)
    {
        ImageView view;

        auto dims = CAMERA_DIMS.proc;

        view.width = dims.width;
        view.height = dims.height;
        view.matrix_data_ = camera.pixels;

        return view;
    }


    static Span32 to_span(ScreenCamera const& camera)
    {
        Span32 view;
        auto dims = CAMERA_DIMS.proc;
        auto length = dims.width * dims.height;

        return span::make_view(camera.pixels, length);
    }
}


/* draw */

namespace game_punk
{
    class DrawQueue
    {
    public:

        u32 capacity = 0;
        u32 size = 0;

        SubView* src;
        SubView* dst;
    };


    void count_draw(DrawQueue& dq, MemoryCounts& counts, u32 capacity)
    {
        dq.capacity = capacity;
        add_count<SubView>(counts, 2 * capacity);
    }


    bool create_draw(DrawQueue& dq, Memory& mem)
    {      
        if (!dq.capacity)
        {
            app_assert("DrawQueue not initialized" && false);
            return false;
        }

        auto res_src = push_mem<SubView>(mem, dq.capacity);
        auto res_dst = push_mem<SubView>(mem, dq.capacity);

        auto ok = res_src.ok && res_dst.ok;

        if (ok)
        {
            dq.src = res_src.data;
            dq.dst = res_dst.data;
        }

        return ok;
    }


    static void draw(DrawQueue const& dq)
    {
        for (u32 i = 0; i < dq.size; i++)
        {
            img::copy_if_alpha(dq.src[i], dq.dst[i]);
        }
    }


    static void reset_draw(DrawQueue& dq)
    {
        dq.size = 0;
    }


    static void push_draw_view(DrawQueue& dq, ImageView const& bmp, ImageView const& out, Point2Di32 out_pos)
    {
        if (!bmp.matrix_data_)
        {
            return;
        }

        i32 w = (i32)out.width;
        i32 h = (i32)out.height;
        i32 x = out_pos.x;
        i32 y = out_pos.y;

        Rect2Di32 screen_rect{};
        screen_rect.x_begin = 0;
        screen_rect.y_begin = 0;
        screen_rect.x_end = w;
        screen_rect.y_end = h;

        Rect2Di32 dst_rect{};
        dst_rect.x_begin = x;
        dst_rect.y_begin = y;
        dst_rect.x_end = x + bmp.width;
        dst_rect.y_end = y + bmp.height;

        if (!rect_intersect(screen_rect, dst_rect))
        {
            return;
        }

        auto dr = clamp_rect(dst_rect, screen_rect);

        Rect2Du32 sr{};
        sr.x_begin = (u32)math::max(0 - x, 0);
        sr.y_begin = (u32)math::max(0 - y, 0);
        sr.x_end = sr.x_begin + dr.x_end - dr.x_begin;
        sr.y_end = sr.y_begin + dr.y_end - dr.y_begin;

        auto i = dq.size;
        dq.size++;

        app_assert(dq.size <= dq.capacity && "Draw capacity");

        dq.src[i] = img::sub_view(bmp, sr);
        dq.dst[i] = img::sub_view(out, dr);
    }
   

    static void push_draw(DrawQueue& dq, BackgroundView const& bg, ScreenCamera const& camera)
    {
        auto bmp = to_image_view(bg);
        auto out = to_image_view(camera);

        BackgroundPosition pos(0, 0, DimCtx::Proc);

        auto p = delta_pos_px(pos, camera.bg_pos);

        push_draw_view(dq, bmp, out, p);
    }
    
    
    static void push_draw(DrawQueue& dq, SpriteView const& sprite, BackgroundPosition pos, ScreenCamera const& camera)
    {
        auto bmp = to_image_view(sprite);
        auto out = to_image_view(camera);
        auto p = delta_pos_px(pos, camera.bg_pos);

        push_draw_view(dq, bmp, out, p);
    }


    static void push_draw(DrawQueue& dq, TileView const& tile, BackgroundPosition pos, ScreenCamera const& camera)
    {
        auto bmp = to_image_view(tile);
        auto out = to_image_view(camera);
        auto p = delta_pos_px(pos, camera.bg_pos);

        push_draw_view(dq, bmp, out, p);
    }


    static void push_draw(DrawQueue& dq, BackgroundPartPair const& pair, ScreenCamera const& camera)
    {
        auto out = to_image_view(camera);

        auto pos = BackgroundPosition(0, 0, DimCtx::Proc);
        auto p = delta_pos_px(pos, camera.bg_pos);
        auto bmp = to_image_view_first(pair);

        push_draw_view(dq, bmp, out, p);

        pos.proc.y = pair.height1;
        p = delta_pos_px(pos, camera.bg_pos);
        bmp = to_image_view_second(pair);
        if (bmp.height)
        {
            push_draw_view(dq, bmp, out, p);
        }
    }
}


/* load assets */

namespace game_punk
{
    class LoadImageQueue
    {
    public:
        u32 capacity = 0;
        u32 size = 0;

        
    };
}


/* input command */

namespace game_punk
{
    class InputCommand
    {
    public:
        union
        {
            b32 move = 0;

            struct
            {
                b8 north;
                b8 south;
                b8 east;
                b8 west;
            };

        } camera;

        union
        {
            b32 changed = 0;

            struct 
            {
                b8 up;
            };

        } text;


        union
        {
            b32 move = 0;

            struct
            {
                b8 north;
                b8 south;
                b8 east;
                b8 west;
            };

        } icon;
    };


    static InputCommand map_input(Input const& input)
    {
        InputCommand cmd;

        cmd.camera.move = 0;
        /*cmd.camera.north = input.keyboard.kbd_up.is_down;
        cmd.camera.south = input.keyboard.kbd_down.is_down;
        cmd.camera.east = input.keyboard.kbd_right.is_down;
        cmd.camera.west = input.keyboard.kbd_left.is_down;*/

        cmd.text.changed = 0;
        cmd.text.up = input.keyboard.npd_plus.pressed;

        cmd.icon.move = 0;
        cmd.icon.north = input.keyboard.kbd_up.pressed;
        cmd.icon.south = input.keyboard.kbd_down.pressed;
        cmd.icon.east = input.keyboard.kbd_right.pressed;
        cmd.icon.west = input.keyboard.kbd_left.pressed;

        return cmd;
    }
}