#pragma once

#include "../../res/xbin/bin_table.hpp"


/* asset constants */

namespace game_punk
{
    namespace bt = bin_table;


namespace cxpr
{

    constexpr Vec2Du32 process_dimensions()
    {
        bt::Background_Bg1 list;

        Vec2Du32 dims;
        
        dims.x = list.items[0].width;
        dims.y = list.items[0].height;

        return dims;
    }
    
    constexpr u32 PROCESS_WIDTH_PX = process_dimensions().x;
    constexpr u32 PROCESS_HEIGHT_PX = process_dimensions().y;


    // rotated
    constexpr u32 GAME_WIDTH_PX = process_dimensions().y;
    constexpr u32 GAME_HEIGHT_PX = process_dimensions().x;


    constexpr u32 find_4k_scale()
    {
        auto w = WIDTH_4K;
        u32 n = 2;
        while (w >= GAME_WIDTH_PX)
        {
            w = WIDTH_4K / n++;
        }        

        return n + 1;
    }

    constexpr u32 SCALE_4K = find_4k_scale();

    constexpr u32 CAMERA_GAME_WIDTH_PX = WIDTH_4K / SCALE_4K;
    constexpr u32 CAMERA_GAME_HEIGHT_PX = HEIGHT_4K / SCALE_4K;


    constexpr Vec2Du32 sky_overlay_dimensions()
    {
        bt::InfoList_Image_Sky_Overlay list;

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



} // cxpr    

}


/* helpers */

namespace game_punk
{
    static Vec2Du32 get_image_dims(bt::FileInfo_Image const& info)
    {
        Vec2Du32 dims;
        dims.x = info.width;
        dims.y = info.height;

        return dims;
    }


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
        p32 pd;

        for (u32 i = 0; i < src.length; i++)
        {
            ps = src.data[i];
            pd = dst.data[i];

            pd.red += ps.red;
            pd.green += ps.green;
            pd.blue += ps.blue;

            dst.data[i] = pd;
        }
    }


    static void mask_fill(Span32 const& dst, p32 color)
    {
        color.alpha = 255; // no transparency allowed

        for (u32 i = 0; i < dst.length; i++)
        {
            dst.data[i] = dst.data[i].alpha ? color : COLOR_TRANSPARENT;
        }
    }


    static void mask_fill(ImageView const& dst_view, p32 color)
    {
        auto dst = img::to_span(dst_view);

        mask_fill(dst, color);
    }


    static void filter_fill(Span32 const& dst, p32 primary, p32 secondary)
    {
        primary.alpha = 255; // no transparency allowed
        secondary.alpha = 255;

        auto blend_r = (primary.red + secondary.red) / 2;
        auto blend_g = (primary.green + secondary.green) / 2;
        auto blend_b = (primary.blue + secondary.blue) / 2;
        auto blend = img::to_pixel((u8)blend_r, (u8)blend_g, (u8)blend_b);

        p32 ps;

        for (u32 i = 0; i < dst.length; i++)
        {
            ps = dst.data[i];

            switch (ps.alpha)
            {
            case 0:
                dst.data[i] = COLOR_TRANSPARENT;
                break;

            case 50:
                dst.data[i] = secondary;
                break;

            case 128:
                dst.data[i] = blend;
                break;

            case 255:
                dst.data[i] = primary;
                break;

            default:
                break;
            }

            dst.data[i].alpha = ps.alpha;
        }
    }


    static void filter_fill(ImageView const& dst_view, p32 primary, p32 secondary)
    {
        auto dst = img::to_span(dst_view);

        filter_fill(dst, primary, secondary);
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


    static void start_random_frame(Randomf32& rng)
    {
        for (u8 i = rng.b_cursor; i < rng.r_cursor; i++)
        {
            rng.values[i] = math::rand(0.0f, 1.0f);
        }

        rng.b_cursor = rng.r_cursor;
    }


    template <typename T>
    T next_random(Randomf32& rng, T min, T max)
    {
        auto val = rng.values[rng.r_cursor++];
        app_assert(rng.r_cursor != rng.b_cursor && "*** Frame RNG exceded ***");

        auto delta = val * (max - min);

        return min + (T)delta;
    }
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


        static TickQty32 get_random(Randomf32& rng, u32 min, u32 max) { return TickQty32(next_random(rng, min, max)); }
    };


    GameTick64 operator + (GameTick64 lhs, TickQty32 rhs) { return GameTick64::make(lhs.value_ + rhs.value_); }

    TickQty32 operator - (GameTick64 lhs, TickQty32 rhs) { return lhs.value_ - rhs.value_; }

    TickQty32 operator % (GameTick64 lhs, TickQty32 rhs) { return lhs.value_ % rhs.value_; }


    bool operator <= (TickQty32 lhs, GameTick64 rhs) { return lhs.value_ <= rhs.value_; }

    //bool operator >= (TickQty lhs, u32 rhs) { return lhs.value_ >= (u64)rhs; }

    TickQty32 operator + (TickQty32 lhs, TickQty32 rhs) { return lhs.value_ + rhs.value_; }

    TickQty32 operator - (TickQty32 lhs, TickQty32 rhs) { return lhs.value_ - rhs.value_; }


    class ActiveRef
    {
    private:
        u8* ref_ = 0;

    public:        
        void set_ref(u8* ref) { ref_ = ref; }

        void set_active(u8 active) { if (ref_) *ref_ = active; }

        void set_on() { if (ref_) *ref_ = 1; }
        void set_off() { if (ref_) *ref_ = 0; }

        bool is_set() { return ref_ && *ref_; }
        bool is_set() const { return ref_ && *ref_; }
    };
    
}


/* orientation context */

namespace game_punk
{
    class ContextDims
    {
    public:
        union
        {
            struct { u32 width; u32 height; } proc;

            struct { u32 height; u32 width; } game;

            u64 any = 0;
        };
    };


    static constexpr ContextDims make_dims_proc(u32 width, u32 height)
    {
        ContextDims dims;

        auto& ctx = dims.proc;
        ctx.width = width;
        ctx.height = height;

        return dims;
    }


    static constexpr ContextDims make_dims_game(u32 width, u32 height)
    {
        ContextDims dims;

        auto& ctx = dims.game;
        ctx.width = width;
        ctx.height = height;

        return dims;
    }


    template <typename T>
    class ContextVec2D
    {
    public:
        union
        {
            struct { T x; T y; } proc;

            struct { T y; T x; } game;
        };
    };

    using CtxPt2Du32 = ContextVec2D<u32>;
    using CtxPt2Di32 = ContextVec2D<i32>;


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
}


/* render view */

namespace game_punk
{    
    class RenderView
    {
    public:
        static constexpr u32 width = cxpr::PROCESS_WIDTH_PX;
        static constexpr u32 height = cxpr::PROCESS_HEIGHT_PX;

        p32* data = 0;
    };


    static void count_view(RenderView& view, MemoryCounts& counts)
    {
        auto length = view.width * view.height;
        add_count<p32>(counts, length);
    }


    static bool create_view(RenderView& view, Memory& memory)
    {
        auto length = view.width * view.height;

        auto res = push_mem<p32>(memory, length);
        if (res.ok)
        {
            view.data = res.data;
        }

        return res.ok;
    }


    static Span32 to_span(RenderView const& view)
    {
        auto length = view.width * view.height;
        return span::make_view(view.data, length);
    }


    static ImageView to_image_view(RenderView const& view)
    {
        return img::make_view(view.width, view.height, view.data);
    }


    static void copy(RenderView const& src, RenderView const& dst)
    {
        span::copy(to_span(src), to_span(dst));
    }


    static void add_pma(RenderView const& src, RenderView const& dst)
    {
        add_pma(to_span(src), to_span(dst));
    }
}


/* render layer */

namespace game_punk
{
    class RenderLayer
    {
    public:

        static constexpr ContextDims dims = make_dims_proc(cxpr::PROCESS_WIDTH_PX, cxpr::PROCESS_HEIGHT_PX);

        p32* data = 0;

        ActiveRef active;
    };


    static void count_render_layer(RenderLayer& layer, MemoryCounts& counts)
    {
        auto& dims = layer.dims.proc;
        auto length = dims.width * dims.height;

        add_count<p32>(counts, length);
    }


    static bool create_render_layer(RenderLayer& layer, Memory& memory)
    {
        auto& dims = layer.dims.proc;
        auto length = dims.width * dims.height;

        auto res = push_mem<p32>(memory, length);
        if (res.ok)
        {
            layer.data = res.data;
        }

        return res.ok;
    }


    static bool layer_active(RenderLayer const& layer)
    {
        return layer.active.is_set();
    }


    static ImageView to_image_view(RenderLayer const& layer)
    {
        auto& dims = layer.dims.proc;

        return img::make_view(dims.width, dims.height, layer.data);
    }


    static Span32 to_span(RenderLayer const& layer)
    {
        auto& ctx = layer.dims.proc;
        auto length = ctx.width * ctx.height;

        return span::make_view(layer.data, length);
    }


    static void clear_render_layer(RenderLayer const& layer)
    {
        span::fill(to_span(layer), COLOR_TRANSPARENT);
    }


    static void copy(RenderView const& src, RenderLayer const& layer)
    {
        img::copy(to_image_view(src), to_image_view(layer));
    }

}


/* camera layer */

namespace game_punk
{
    class CameraLayer
    {
    public:

        static constexpr ContextDims dims = make_dims_game(cxpr::CAMERA_GAME_WIDTH_PX, cxpr::CAMERA_GAME_HEIGHT_PX);

        p32* data = 0;

        ActiveRef active;
    };


    static void count_camera_layer(CameraLayer& layer, MemoryCounts& counts)
    {
        auto& dims = layer.dims.proc;
        auto length = dims.width * dims.height;

        add_count<p32>(counts, length);
    }


    static bool create_camera_layer(CameraLayer& layer, Memory& memory)
    {
        auto& dims = layer.dims.proc;
        auto length = dims.width * dims.height;

        auto res = push_mem<p32>(memory, length);
        if (res.ok)
        {
            layer.data = res.data;
        }

        return res.ok;
    }


    static bool layer_active(CameraLayer const& layer)
    {
        return layer.active.is_set();
    }


    static ImageView to_image_view(CameraLayer const& layer)
    {
        auto& dims = layer.dims.proc;

        return img::make_view(dims.width, dims.height, layer.data);
    }
    
    
    static Span32 to_span(CameraLayer const& layer)
    {
        auto& ctx = layer.dims.proc;
        auto length = ctx.width * ctx.height;

        return span::make_view(layer.data, length);
    }


    static void clear_camera_layer(CameraLayer const& layer)
    {
        span::fill(to_span(layer), COLOR_TRANSPARENT);
    }
}


/* asset data */

namespace game_punk
{
    class AssetData
    {
    public:

        cstr bin_file_path = 0;

        MemoryBuffer<u8> bytes;
    };


    static void destroy_asset_data(AssetData& gd)
    {
        mb::destroy_buffer(gd.bytes);
    }
}


/* sky overlay view */

namespace game_punk
{
    class SkyOverlayView
    {
    public:
        static constexpr u32 width = cxpr::SKY_OVERLAY_WIDTH_PX;
        static constexpr u32 height = cxpr::SKY_OVERLAY_HEIGHT_PX;

        p32* data = 0;
    };


    static void count_view(SkyOverlayView& view, MemoryCounts& counts)
    {
        auto length = view.width * view.height;
        add_count<p32>(counts, length);
    }


    static bool create_view(SkyOverlayView& view, Memory& memory)
    {
        if (!view.height)
        {
            app_assert("SkyOverlayView not initialized" && false);
            return false;
        }

        auto length = view.width * view.height;

        auto res = push_mem<p32>(memory, length);
        if (res.ok)
        {
            view.data = res.data;
        }

        return res.ok;
    }


    static Span32 to_span(SkyOverlayView const& view)
    {
        auto length = view.width * view.height;
        return span::make_view(view.data, length);
    }


    static ImageView to_image_view(SkyOverlayView const& view)
    {
        return img::make_view(view.width, view.height, view.data);
    }


    static SubView sub_view(SkyOverlayView const& view, Rect2Du32 const& rect)
    {
        return img::sub_view(to_image_view(view), rect);
    }
}


/* background images */

namespace game_punk
{
    class BackgroundState
    {
    public:

        static constexpr ContextDims dims = make_dims_game(cxpr::GAME_WIDTH_PX, cxpr::GAME_HEIGHT_PX);

        

        struct 
        {
            RenderView sky_base;
            SkyOverlayView sky_overlay;

            RenderView bg_1;
            RenderView bg_2;

        } data;

        RenderView ov;
        Point2Du32 ov_pos;
        Vec2Di32 ov_vel;

        RenderView sky;

        RenderLayer layer_sky;
        RenderLayer layer_bg_1;
        RenderLayer layer_bg_2;        
    };


    static void reset_background_state(BackgroundState& bg_state)
    {
        bool ok = true;
        ok &= !(!bg_state.data.sky_base.data);
        ok &= !(!bg_state.data.sky_overlay.data);

        ok &= !(!bg_state.data.bg_1.data);
        ok &= !(!bg_state.data.bg_2.data);

        ok &= !(!bg_state.ov.data);

        ok &= !(!bg_state.sky.data);

        ok &= !(!bg_state.layer_sky.data);
        ok &= !(!bg_state.layer_bg_1.data);
        ok &= !(!bg_state.layer_bg_2.data);

        app_assert(ok && "*** BackgroundState not created ***");

        bg_state.ov_pos = { 0, 0 };
        bg_state.ov_vel = { 2, 1 };

        auto dst = to_image_view(bg_state.ov);
        auto w = dst.width;
        auto h = dst.height;

        auto& pos = bg_state.ov_pos;
        
        auto rect = img::make_rect(pos.x, pos.y, w, h);
        auto src = sub_view(bg_state.data.sky_overlay, rect);

        img::copy(src, dst);

        copy(bg_state.data.sky_base, bg_state.sky);
        add_pma(bg_state.ov, bg_state.sky);
        copy(bg_state.sky, bg_state.layer_sky);
        copy(bg_state.data.sky_base, bg_state.sky);
    }


    static void count_background_state(BackgroundState& bg_state, MemoryCounts& counts)
    {    
        auto& ctx = bg_state.dims.proc;
        Vec2Du32 dims = { ctx.width, ctx.height };

        count_view(bg_state.data.sky_base, counts);

        constexpr auto ov_dims = cxpr::sky_overlay_dimensions();

        count_view(bg_state.data.sky_overlay, counts);
        count_view(bg_state.data.bg_1, counts);
        count_view(bg_state.data.bg_2, counts);

        count_view(bg_state.ov, counts);

        count_view(bg_state.sky, counts);
        
        count_render_layer(bg_state.layer_sky, counts);
        count_render_layer(bg_state.layer_bg_1, counts);
        count_render_layer(bg_state.layer_bg_2, counts);
    }


    static bool create_background_state(BackgroundState& bg_state, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(bg_state.data.sky_base, memory);
        ok &= create_view(bg_state.data.sky_overlay, memory);
        ok &= create_view(bg_state.data.bg_1, memory);
        ok &= create_view(bg_state.data.bg_2, memory);

        ok &= create_view(bg_state.ov, memory);

        ok &= create_view(bg_state.sky, memory);
        
        ok &= create_render_layer(bg_state.layer_sky, memory);
        ok &= create_render_layer(bg_state.layer_bg_1, memory);
        ok &= create_render_layer(bg_state.layer_bg_2, memory);

        return ok;
    }


    static void update_sky_overlay(BackgroundState& bg_state)
    {
        auto dst = to_image_view(bg_state.ov);
        auto w = dst.width;
        auto h = dst.height;

        auto& data = bg_state.data.sky_overlay;

        i32 xm = data.width - w - 1;
        i32 ym = data.height - h - 1;

        auto& pos = bg_state.ov_pos;
        auto& vel = bg_state.ov_vel;

        i32 x = pos.x + vel.x;
        i32 y = pos.y + vel.y;

        if (x < 0 || x > xm)
        {
            vel.x *= -1;
        }
        else
        {
            pos.x = (u32)x;
        }

        if (y < 0 || y > ym)
        {
            vel.y *= -1;
        }
        else
        {
            pos.y = (u32)y;
        }
        
        auto rect = img::make_rect(pos.x, pos.y, w, h);
        auto src = sub_view(data, rect);

        img::copy(src, dst);
        
    }
    
    
    static void render_background_sky(BackgroundState& bg_state, GameTick64 game_tick)
    {
        constexpr u32 frame_wait = 6;

        auto t = game_tick.value_ % frame_wait;

        auto const t_pma = [&]()
        {
            auto ov = to_span(bg_state.ov);
            auto sky = to_span(bg_state.sky);
            u32 length = sky.length / (frame_wait - 2);
            u32 offset = (t - 1) * length;
            auto src = span::sub_view(ov, offset, length);
            auto dst = span::sub_view(sky, offset, length);
            add_pma(src, dst);
        };

        switch (t)
        {
        case 0:
            update_sky_overlay(bg_state);
            copy(bg_state.data.sky_base, bg_state.sky);
            break;

        case (frame_wait - 1):
            // write to render layer
            copy(bg_state.sky, bg_state.layer_sky);
            break;

        default: 
            t_pma();
            break;
        }        
    }
    

}


/* spritesheet */

namespace game_punk
{
    class SpritesheetView
    {
    public:
        ContextDims dims;

        p32* data = 0;

        ContextDims bitmap_dims;
        u32 bitmap_count = 0;
    };


    static void count_view(SpritesheetView& view, MemoryCounts& counts, bt::FileInfo_Image const& info, u32 count_h = 0)
    {
        bool ok = info.type == bt::FileType::Image1C;
        app_assert(ok && "*** Unexpected spritesheet image ***");

        auto& ctx = view.dims.proc;

        ctx.width = info.width;
        ctx.height = info.height;

        auto& bmp = view.bitmap_dims.proc;
        bmp.width = ctx.width;

        if (count_h == 0)
        {
            // assume width == height            
            bmp.height = ctx.width;
            view.bitmap_count = ctx.height / ctx.width;
        }
        else
        {
            // equal height
            view.bitmap_count = count_h;
            bmp.height = ctx.height / view.bitmap_count;
        }

        auto length = ctx.width * ctx.height;

        add_count<p32>(counts, length);
    }


    static bool create_view(SpritesheetView& view, Memory& memory)
    {
        auto& ctx = view.dims.proc;

        if (!view.dims.any || !view.bitmap_dims.any || !view.bitmap_count)
        {
            app_assert("SpritesheetView not initialized" && false);
            return false;
        }

        auto length = ctx.width * ctx.height;

        auto res = push_mem<p32>(memory, length);
        if (res.ok)
        {
            view.data = res.data;
        }

        return res.ok;
    }


    static Span32 to_span(SpritesheetView const& view)
    {
        auto dims = view.dims.proc;

        auto length = dims.width * dims.height;
        
        return span::make_view(view.data, length);
    }


    static ImageView to_image_view(SpritesheetView const& view)
    {
        auto dims = view.dims.proc;
        return img::make_view(dims.width, dims.height, view.data);
    }
}


/* sprite view */

namespace game_punk
{
    class SpriteView
    {
    public:
        ContextDims dims;

        p32* data;
    };


    static ImageView to_image_view(SpriteView const& view)
    {
        auto ctx = view.dims.proc;

        return img::make_view(ctx.width, ctx.height, view.data);
    }
}


/* spritesheet state */

namespace game_punk
{
    class SpritesheetState
    {
    public:        

        struct
        {
            SpritesheetView punk_run;

        } data;

        RenderLayer layer_sprite;        
    };


    static void count_spritesheet_state(SpritesheetState& ss_state, MemoryCounts& counts)
    {
        bt::Spriteset_Punk list;

        count_view(ss_state.data.punk_run, counts, list.file_info.Punk_run);
        count_render_layer(ss_state.layer_sprite, counts);
    }


    static bool create_spritesheet_state(SpritesheetState& ss_state, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(ss_state.data.punk_run, memory);
        ok &= create_render_layer(ss_state.layer_sprite, memory);

        return ok;
    }
}


/* tiles */

namespace game_punk
{
    class TileView
    {
    public:
        ContextDims dims;

        p32* data;
    };


    static void count_view(TileView& view, MemoryCounts& counts, bt::FileInfo_Image const& info)
    {
        bool ok = info.type == bt::FileType::Image1C;
        app_assert(ok && "*** Unexpected tile image ***");

        auto& ctx = view.dims.proc;

        ctx.width = info.width;
        ctx.height = info.height;

        auto length = ctx.width * ctx.height;

        add_count<p32>(counts, length);
    }


    static bool create_view(TileView& view, Memory& memory)
    {
        if (!view.dims.any)
        {
            app_assert("TileView not initialized" && false);
            return false;
        }

        auto& ctx = view.dims.proc;

        auto length = ctx.width * ctx.height;

        auto res = push_mem<p32>(memory, length);
        if (res.ok)
        {
            view.data = res.data;
        }

        return res.ok;
    }


    static Span32 to_span(TileView const& view)
    {
        auto length = view.dims.proc.width * view.dims.proc.height;

        return span::make_view(view.data, length);
    }


    static ImageView to_image_view(TileView const& view)
    {
        auto dims = view.dims.proc;

        return img::make_view(dims.width, dims.height, view.data);
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
        bt::Tileset_ex_zone list;
        count_view(tiles.floor_a, counts, list.file_info.floor_02);
        count_view(tiles.floor_b, counts, list.file_info.floor_03);
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
            
            SpritesheetView font;
            SpritesheetView icons;

            p32 colors[CTS];
        } data;

        MemoryStack<p32> pixels;

        CameraLayer ui;
        CameraLayer hud;

        u8 font_color_id;

        struct 
        {
            b8 is_on;
            GameTick64 end_tick;

        } temp_icon;

        
    };


    static void count_ui_state(UIState& ui, MemoryCounts& counts)
    {
        bt::UIset_Title title;
        auto t_info = title.file_info.title_main;
        count_view(ui.data.title, counts, t_info.width, t_info.height);        
        
        bt::UIset_Font font;
        u32 n_chars = 10 + 26 * 2; // 0-9, A-Z x 2
        count_view(ui.data.font, counts, font.file_info.font, n_chars);

        bt::UIset_Icons icons;
        count_view(ui.data.icons, counts, icons.file_info.icons);

        auto length = cxpr::CAMERA_GAME_WIDTH_PX * cxpr::CAMERA_GAME_HEIGHT_PX;
        count_stack(ui.pixels, counts, length);

        count_camera_layer(ui.ui, counts);
        count_camera_layer(ui.hud, counts);
    }


    static bool create_ui_state(UIState& ui, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(ui.data.title, memory);
        ok &= create_view(ui.data.font, memory);
        ok &= create_view(ui.data.icons, memory);
        ok &= create_stack(ui.pixels, memory);
        ok &= create_camera_layer(ui.ui, memory);
        ok &= create_camera_layer(ui.hud, memory);        

        return ok;
    }


    static void reset_ui_state(UIState& ui)
    {
        ui.temp_icon.is_on = 0;
        ui.temp_icon.end_tick = GameTick64::make(1);
    }


    static void start_ui_frame(UIState& ui)
    {
        clear_camera_layer(ui.ui);
        clear_camera_layer(ui.hud);
        reset_stack(ui.pixels);
        span::fill(to_span(ui.pixels), COLOR_TRANSPARENT);
    }


    static bool set_ui_color(UIState& ui, u8 color_id)
    {
        bool ok = color_id < ui.CTS;
        app_assert(ok && "*** Invalid color id ***");

        auto color = ui.data.colors[color_id];
        mask_fill(to_image_view(ui.data.font), color);

        // temp icon color
        filter_fill(to_image_view(ui.data.icons), color, COLOR_BLACK);
        ui.font_color_id = color_id;

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

            auto delta = icon.is_on ? TickQty32::get_random(rng, 3, 60) : TickQty32::make(6);
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


/* game camera */

namespace game_punk
{ 
    
    class GameCamera
    {
    public:

        static constexpr ContextDims render_dims_px = RenderLayer::dims;

        static constexpr ContextDims viewport_dims_px = CameraLayer::dims;

        CtxPt2Du32 viewport_pos_px;
    };


    static void reset_game_camera(GameCamera& camera)
    {
        camera.viewport_pos_px.proc = { 10, 16 };
    }

    
    static Rect2Du32 get_camera_rect(GameCamera const& camera)
    {
        auto& dims = camera.viewport_dims_px.proc;

        auto pos = camera.viewport_pos_px.proc;
        auto w = dims.width;
        auto h = dims.height;

        return img::make_rect(pos.x, pos.y, w, h);
    }


    static void move_camera(GameCamera& camera, Vec2Di8 delta_px)
    {
        auto& max_dims = camera.render_dims_px.game;
        auto& cam_dims = camera.viewport_dims_px.game;
        auto& pos = camera.viewport_pos_px.game;

        auto x_max = (i32)(max_dims.width - cam_dims.width);
        auto y_max = (i32)(max_dims.height - cam_dims.height);        

        auto pos_x = (i32)pos.x;
        auto pos_y = (i32)pos.y;

        pos_x += delta_px.x;
        pos_y += delta_px.y;

        pos.x = (u32)math::cxpr::clamp(pos_x, 0, x_max);
        pos.y = (u32)math::cxpr::clamp(pos_y, 0, y_max);
    }
}


/* render state */

namespace game_punk
{
    class RenderState
    {
    public:
        static constexpr u32 layer_count = (u32)RenderLayerId::Count;
        static constexpr u32 ui_layer_count = (u32)CameraLayerId::Count;

        p32* layers[layer_count];
        u8 layers_active[layer_count];

        p32* ui_layers[ui_layer_count];
        u8 ui_layers_active[ui_layer_count];

        CameraLayer screen_out;
    };


    static void reset_render_state(RenderState& render)
    {        
        for (u32 i = 0; i < render.layer_count; i++)
        {
            // activate all layers
            render.layers_active[i] = 1;

            // verify all layers have been set
            if (!render.layers[i])
            {
                app_assert(false && "*** Render layer(s) not set ***");
            }
        }

        for (u32 i = 0; i < render.ui_layer_count; i++)
        {
            render.ui_layers_active[i] = 0;

            if (!render.ui_layers[i])
            {
                app_assert(false && "*** UI layer(s) not set ***");
            }
        }

        if (!render.screen_out.data)
        {
            app_assert(false && "*** Screen layer not set ***");
        }

        render.screen_out.active.set_on();
    }


    static void init_render_state(RenderState& render)
    {
        
        for (u32 i = 0; i < render.layer_count; i++)
        {
            // activate all layers
            render.layers_active[i] = 1;

            // invalidate all layers
            render.layers[i] = 0;
        }
        
        for (u32 i = 0; i < render.ui_layer_count; i++)
        {
            render.ui_layers_active[i] = 1;
            render.ui_layers[i] = 0;
        }

        render.screen_out.data = 0;
        render.screen_out.active.set_on();
    }


    static bool set_render_layer(RenderState& render, RenderLayer& layer, RenderLayerId layer_id)
    {
        auto id = (u32)layer_id;

        bool ok = id < render.layer_count;
        app_assert(ok && "*** Invalid layer_id ***");

        ok &= layer.data != 0;
        app_assert(ok && "*** layer memory not set ***");

        if (ok)
        {
            render.layers[id] = layer.data;
            layer.active.set_ref(render.layers_active + id);
        }

        return ok;
    }


    static bool set_ui_layer(RenderState& render, CameraLayer& layer, CameraLayerId layer_id)
    {
        auto id = (u32)layer_id;

        bool ok = id < render.ui_layer_count;
        app_assert(ok && "*** Invalid layer_id ***");

        ok &= layer.data != 0;
        app_assert(ok && "*** ui layer memory not set ***");

        if (ok)
        {
            render.ui_layers[id] = layer.data;
            layer.active.set_ref(render.ui_layers_active + id);
        }

        return ok;
    }


    static bool set_out_layer(RenderState& render, ImageView const& screen)
    {
        bool ok = screen.matrix_data_ != 0;
        app_assert(ok && "*** screen layer memory not set ***");

        auto dims = CameraLayer::dims;
        ok &= screen.width == dims.proc.width;
        ok &= screen.height == dims.proc.height;
        app_assert(ok && "*** Invalid screen size ***");

        if (ok)
        {            
            render.screen_out.data = screen.matrix_data_;
        }

        return ok;
    }


    static void render_to_screen(RenderState& render, GameCamera const& camera)
    {
        constexpr auto NR = render.layer_count;
        constexpr auto NU = render.ui_layer_count;

        auto red = img::to_pixel(255, 0, 0);

        auto layers = render.layers;
        auto ui = render.ui_layers;

        i32 ar[NR];
        i32 au[NU];
        u32 c = 0;
        p32* dst = 0;
        p32 ps;

        // find active ui layers
        c = 0;
        for (u32 id = 0; id < NU; id++)
        {
            au[id] = -1;
            if (render.ui_layers_active[id])
            {
                au[c++] = id;
            }
        }

        // find active render layers
        c = 0;     
        for (u32 id = 0; id < NR; id++)
        {
            ar[id] = -1;
            if (render.layers_active[id])
            {
                ar[c++] = id;
            }
        }

        auto out = to_image_view(render.screen_out);
        auto rect = get_camera_rect(camera);

        u32 stride = RenderLayer::dims.proc.width;
        u32 xb = rect.x_begin;       
        

        for (u32 y = 0; y < out.height; y++)
        {
            dst = img::row_begin(out, y);
            u32 su = y * out.width;
            u32 sr = (rect.y_begin + y) * stride + xb;

            for (u32 x = 0; x < out.width; x++, su++, sr++)
            {
                ps = COLOR_TRANSPARENT;

                // ui layers
                for (c = 0; c < NU && au[c] >= 0; c++)
                {
                    auto id = au[c];
                    ps = ui[id][su];

                    if (ps.alpha)
                    {  
                        dst[x] = ps;
                        break;
                    }
                }

                if (ps.alpha)
                {
                    continue;
                }

                // render layers
                for (c = 0; c < NR && ar[c] >= 0; c++)
                {
                    auto id = ar[c];
                    ps = layers[id][sr];

                    if (ps.alpha)
                    {      
                        dst[x] = ps;
                        break;
                    }
                }
            }
        }

    }
}


/* draw */

namespace game_punk
{
    class DrawQueue
    {
    public:

        static constexpr u32 capacity = 50;
        u32 size = 0;

        SubView src[capacity];
        SubView dst[capacity];
    };


    /*void count_draw(DrawQueue& dq, MemoryCounts& counts, u32 capacity)
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
    }*/


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


    static void push_draw_sprite(DrawQueue& dq, SpriteView const& sprite, RenderLayer const& layer, CtxPt2Di32 pos)
    {
        auto bmp = to_image_view(sprite);
        auto out = to_image_view(layer);
        Point2Di32 p = { pos.proc.x, pos.proc.y };

        push_draw_view(dq, bmp, out, p);
    }


    static void push_draw_tile(DrawQueue& dq, TileView const& tile, RenderLayer const& layer, CtxPt2Di32 pos)
    {
        auto bmp = to_image_view(tile);
        auto out = to_image_view(layer);
        Point2Di32 p = { pos.proc.x, pos.proc.y };

        push_draw_view(dq, bmp, out, p);
    }


    static void push_draw_ui(DrawQueue& dq, SpriteView const& sprite, CameraLayer const& layer, CtxPt2Di32 pos)
    {
        auto bmp = to_image_view(sprite);
        auto out = to_image_view(layer);
        Point2Di32 p = { pos.proc.x, pos.proc.y };

        push_draw_view(dq, bmp, out, p);
    }
}


/* animation */

namespace game_punk
{
    class AnimationFast
    {
    public:

        u32 bitmap_count = 0;

        u32 ticks_per_bitmap;

        p32* spritesheet_data = 0;

        ContextDims bitmap_dims;
    };


    static bool set_animation_spritesheet(AnimationFast& an, SpritesheetView const& ss)
    {
        auto& dims = ss.dims.game;

        bool ok = ss.data != 0;
        app_assert(ok && "*** Spritesheet data not set ***");

        ok &= dims.width > dims.height;
        ok &= dims.width % dims.height == 0;
        app_assert(ok && "*** Invalid spritesheet dimensions ***");

        an.spritesheet_data = ss.data;

        an.bitmap_dims = ss.bitmap_dims;

        an.bitmap_count = ss.bitmap_count;

        an.ticks_per_bitmap = 7; // Magic!

        return ok;
    }


    static SpriteView get_animation_bitmap(AnimationFast& an, auto tick)
    {
        p32* data = 0;

        auto t = tick.value_ % (an.bitmap_count * an.ticks_per_bitmap);

        auto dims = an.bitmap_dims.proc;

        auto b = t / an.ticks_per_bitmap;
        if (b < an.bitmap_count)
        {
            auto offset = b * dims.width * dims.height;
            data = an.spritesheet_data + offset;
        }

        app_assert(data);

        SpriteView view;
        view.dims = an.bitmap_dims;
        view.data = data;

        return view;
    }
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