#pragma once


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


    void count_queue(DrawQueue& dq, MemoryCounts& counts, u32 capacity)
    {
        dq.capacity = capacity;
        add_count<SubView>(counts, 2 * capacity);
    }


    bool create_queue(DrawQueue& dq, Memory& mem)
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