#pragma once

/* sky animation */

namespace game_punk
{
    class SkyAnimation
    {
    public:
        BackgroundView base;
        SkyOverlayView overlay_src;

        BackgroundPosition ov_pos;
        Vec2Di32 ov_vel;
        BackgroundView ov_bg;

        BackgroundView out[2];
        u8 out_id = 0;


        BackgroundView& out_front() { return out[out_id]; }

        BackgroundView& out_back() { return out[!out_id]; }

        void out_swap() { out_id = !out_id; }
    };


    static void count_sky_animation(SkyAnimation& sky, MemoryCounts& counts)
    {
        count_view(sky.base, counts);
        count_view(sky.overlay_src, counts);
        count_view(sky.ov_bg, counts);
        count_view(sky.out[0], counts);
        count_view(sky.out[1], counts);
    }


    static bool create_sky_animation(SkyAnimation& sky, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(sky.base, memory);
        ok &= create_view(sky.overlay_src, memory);
        ok &= create_view(sky.ov_bg, memory);
        ok &= create_view(sky.out[0], memory);
        ok &= create_view(sky.out[1], memory);

        return ok;
    }
    

    static void render_front_back(SkyAnimation& sky)
    {
        auto base = to_span_cx(sky.base);        
        auto ov = to_span_cx(sky.ov_bg);
        auto front = to_span_cx(sky.out_front());
        auto back = to_span_cx(sky.out_back());

        img::copy(sub_view(sky.overlay_src, sky.ov_pos), to_image_view(sky.ov_bg));

        copy(base, front);
        copy(front, back);
    }


    static void reset_sky_animation(SkyAnimation& sky)
    {
        sky.ov_pos.proc = { 0, 0 };
        sky.ov_vel = { 2, 1 };

        bool ok = true;
        ok &= has_data(sky.base);
        ok &= has_data(sky.overlay_src);
        ok &= has_data(sky.ov_bg);
        ok &= has_data(sky.out[0]);
        ok &= has_data(sky.out[1]);

        app_assert(ok && "*** SkyAnimation not created ***");

        render_front_back(sky);
    }


    static void update_overlay(SkyAnimation& sky)
    {
        auto ov = to_image_view(sky.ov_bg);
        auto w = ov.width;
        auto h = ov.height;

        auto& data_ov = sky.overlay_src;

        i32 xm = data_ov.width - w - 1;
        i32 ym = data_ov.height - h - 1;

        auto& pos = sky.ov_pos.proc;
        auto& vel = sky.ov_vel;

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

        img::copy(sub_view(data_ov, pos), ov);
    }
    
    
    static BackgroundView get_sky_animation(SkyAnimation& sky, GameTick64 game_tick)
    {
        constexpr u32 frame_wait = 6;

        auto t = game_tick.value_ % frame_wait;

        auto const render_back_part = [&]()
        {
            constexpr auto N = frame_wait - 2;

            auto ov = to_span_cx(sky.ov_bg);
            auto out = to_span_cx(sky.out_back());

            constexpr auto L1 = ov.length;
            constexpr auto L2 = out.length;
            static_assert(L1 == L2);
            static_assert(L1 % N == 0);

            constexpr u32 length = L1 / N;
            u32 offset = (t - 2) * length;
            auto src = sub_view(ov, offset, length);
            auto dst = sub_view(out, offset, length);
            add_pma(src, dst);
        };

        switch (t)
        {
        case 0:
            // expose for rendering
            sky.out_swap();
            copy(sky.base, sky.out_back());
            break;

        case 1:
            update_overlay(sky);
            break;

        default: 
            render_back_part();
            break;
        }

        return sky.out_front();
    }
    
}


/* background animation */

namespace game_punk
{
    class BackgroundPartPair
    {
    public:

        u32 height1 = 0;
        u32 height2 = 0;

        p32* data1 = 0;
        p32* data2 = 0;
    };


    static ImageView to_image_view_first(BackgroundPartPair const& bp)
    {
        ImageView view;

        view.width = BACKGROUND_DIMS.proc.width;
        view.height = bp.height1;
        view.matrix_data_ = bp.data1;

        return view;
    }


    static ImageView to_image_view_second(BackgroundPartPair const& bp)
    {
        ImageView view;

        view.width = BACKGROUND_DIMS.proc.width;
        view.height = bp.height2;
        view.matrix_data_ = bp.data2;

        return view;
    }


    class BackgroundAnimation
    {
    public:

        BackgroundView data[1];

        p32* list[4] = { 0 };

        u32 shift = 0;        
    };


    static void reset_background_animation(BackgroundAnimation& an)
    {
        bool ok = has_data(an.data[0]);

        app_assert(ok && "*** BackgroundAnimation not created ***");

        an.shift = 0;

        for (u32 i = 0; i < 4; i++)
        {
            an.list[i] = an.data[0].data;
        }
    }


    static void count_background_animation(BackgroundAnimation& an, MemoryCounts& counts)
    {
        count_view(an.data[0], counts);
    }


    static bool create_background_animation(BackgroundAnimation& an, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(an.data[0], memory);

        return ok;
    }


    static BackgroundPartPair get_animation_pair(BackgroundAnimation const& an, u64 pos)
    {
        BackgroundPartPair bp;

        auto W = BACKGROUND_DIMS.proc.width;
        auto H = BACKGROUND_DIMS.proc.height;

        pos <<= an.shift; // speed
        pos %= (4 * H);

        auto list_id1 = pos / H;
        auto list_id2 = (list_id1 + 1) & (4 - 1);

        pos %= H;
        
        bp.height2 = pos;
        bp.height1 = H - bp.height2;

        bp.data1 = an.list[list_id1] + bp.height2 * W;
        bp.data2 = an.list[list_id2];

        return bp;
    }
}


/* sprite animation */

namespace game_punk
{
    class SpriteAnimation
    {
    public:

        u32 bitmap_count = 0;

        u32 ticks_per_bitmap;

        p32* spritesheet_data = 0;

        ContextDims bitmap_dims;
    };


    static bool set_animation_spritesheet(SpriteAnimation& an, SpritesheetView const& ss)
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


    static SpriteView get_animation_bitmap(SpriteAnimation const& an, u64 pos)
    {
        p32* data = 0;

        auto t = pos % (an.bitmap_count * an.ticks_per_bitmap);

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