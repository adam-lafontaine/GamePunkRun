#pragma once


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
        bool ok = bt::data_size(info.type) == bt::data_size(bt::FileType::Image1C);
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
        bool ok = bt::data_size(info.type) == bt::data_size(bt::FileType::Image1C);

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