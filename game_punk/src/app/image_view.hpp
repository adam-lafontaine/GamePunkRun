#pragma once


/* sprite view */

namespace game_punk
{
    class GameImageView
    {
    public:
        ContextDims dims;

        p32* data;
    };


    static ImageView to_image_view(GameImageView const& view)
    {
        auto ctx = view.dims.proc;

        return img::make_view(ctx.width, ctx.height, view.data);
    }
    
    
    class SpriteView : public GameImageView {};
}


/* tile view */

namespace game_punk
{
    class TileView : public GameImageView {};


    static void count_view(TileView& view, MemoryCounts& counts, auto const& info)
    {
        bool ok = info.type == bt::FileType::Image1C_Table;
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


/* background view */

namespace game_punk
{    
    class BackgroundView
    {
    public:
        static constexpr auto dims = BACKGROUND_DIMS;

        p32* data = 0;
    };


    static void count_view(BackgroundView& view, MemoryCounts& counts)
    {
        auto length = view.dims.proc.width * view.dims.proc.height;
        add_count<p32>(counts, length);
        view.data = 0;
    }


    static bool create_view(BackgroundView& view, Memory& memory)
    {
        auto length = view.dims.proc.width * view.dims.proc.height;

        auto res = push_mem<p32>(memory, length);
        if (res.ok)
        {
            view.data = res.data;
        }

        return res.ok;
    }


    static auto to_span_cx(BackgroundView const& view)
    {      
        constexpr auto W = cxpr::GAME_BACKGROUND_WIDTH_PX;
        constexpr auto H = cxpr::GAME_BACKGROUND_HEIGHT_PX;
        return Span32_cxpr<W, H>(view.data);
    }


    static Span32 to_span(BackgroundView const& view)
    {
        auto length = view.dims.proc.width * view.dims.proc.height;
        return span::make_view(view.data, length);
    }


    static ImageView to_image_view(BackgroundView const& view)
    {
        auto dims = view.dims.proc;
        return img::make_view(dims.width, dims.height, view.data);
    }


    static void copy(BackgroundView const& src, BackgroundView const& dst)
    {
        copy(to_span_cx(src), to_span_cx(dst));
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


    static auto to_span_cx(SkyOverlayView const& view)
    {
        constexpr auto W = cxpr::SKY_OVERLAY_WIDTH_PX;
        constexpr auto H = cxpr::SKY_OVERLAY_HEIGHT_PX;
        return Span32_cxpr<W, H>(view.data);
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


    static SubView sub_view(SkyOverlayView const& view, Vec2Di32 pos)
    {
        auto x = (u32)pos.x;
        auto y = (u32)pos.y;
        auto w = BACKGROUND_DIMS.proc.width;
        auto h = BACKGROUND_DIMS.proc.height;
        return img::sub_view(to_image_view(view), img::make_rect(x, y, w, h));
    }


    static SubView sub_view(SkyOverlayView const& view, BackgroundPosition pos)
    {
        return sub_view(view, pos.proc);
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


    static void count_view(SpritesheetView& view, MemoryCounts& counts, auto const& info, u32 count_h = 0)
    {
        bool ok = info.type == bt::FileType::Image1C_Table || info.type == bt::FileType::Image1C_Filter;
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