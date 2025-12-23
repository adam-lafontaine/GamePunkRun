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

    
    class BackgroundAnimationFast
    {
    public:

        u32 count = 0;
        u32 speed_shift = 0;

        BackgroundView data[cxpr::BACKGROUND_COUNT_MAX]; // Fast, uses more memory

        u8 work_ids[4] = {0};
        u8 select_ids[cxpr::BACKGROUND_COUNT_MAX - 4] = {0};        

        u32 work_next = 0;
    };


    static void reset_background_animation(BackgroundAnimationFast& an)
    {
        bool ok = has_data(an.data[0]);

        app_assert(ok && "*** BackgroundAnimationFast not created ***");

        an.speed_shift = 0;

        for (u32 i = 0; i < 4; i++)
        {
            an.work_ids[i] = i;
        }

        for (u32 i = 4; i < an.count; i++)
        {
            an.select_ids[i - 4] = i;
        }
    }


    static void count_background_animation(BackgroundAnimationFast& an, MemoryCounts& counts, u32 n_backgrounds)
    {
        app_assert(n_backgrounds <= cxpr::BACKGROUND_COUNT_MAX);

        an.count = n_backgrounds;

        for (u32 i = 0; i < an.count; i++)
        {
            count_view(an.data[i], counts);
        }        
    }


    static bool create_background_animation(BackgroundAnimationFast& an, Memory& memory)
    {
        bool ok = true;

        for (u32 i = 0; i < an.count; i++)
        {
            ok &= create_view(an.data[i], memory);
        }

        return ok;
    }


    static BackgroundPartPair get_animation_pair(BackgroundAnimationFast& an, Randomf32& rng, u64 pos)
    {
        BackgroundPartPair bp;

        auto W = BACKGROUND_DIMS.proc.width;
        auto H = BACKGROUND_DIMS.proc.height;

        pos <<= an.speed_shift; // speed
        pos %= (4 * H);

        u32 work_1 = pos / H;
        u32 work_2 = (work_1 + 1) & (4 - 1);

        pos %= H;
        
        bp.height2 = pos;
        bp.height1 = H - bp.height2;

        bp.data1 = an.data[an.work_ids[work_1]].data + bp.height2 * W;
        bp.data2 = an.data[an.work_ids[work_2]].data;

        if (bp.height2 == 0)
        { 
            auto select = next_random_u32(rng, 0, an.count - 4 - 1);
            auto bg_id = an.select_ids[select];

            an.select_ids[select] = an.work_ids[an.work_next];
            an.work_ids[an.work_next] = bg_id;

            an.work_next = work_1;
        }

        return bp;
    }
}


namespace game_punk
{
    class BackgroundAnimation
    {
    public:
        using ImageInfo = bt::Background_Bg1::FileInfo;

        static constexpr u32 work_count = 4;
        static constexpr u32 data_count = 2;

        u32 asset_count = 0;

        u32 select_count = 0;
        
        u32 speed_shift = 0;

        ImageInfo asset_info[cxpr::BACKGROUND_COUNT_MAX];
        Span32 table;

        BackgroundView data[data_count];

        u8 work_ids[work_count] = {0};
        u8 select_ids[cxpr::BACKGROUND_COUNT_MAX - work_count] = {0};        

        u32 work_next = 0;
    };


    static void reset_background_animation(BackgroundAnimation& an)
    {
        bool ok = has_data(an.data[0]) && has_data(an.table);

        app_assert(ok && "*** BackgroundAnimation not created ***");

        an.speed_shift = 0;

        for (u32 i = 0; i < an.work_count; i++)
        {
            an.work_ids[i] = i;
        }

        for (u32 i = an.work_count; i < an.asset_count; i++)
        {
            an.select_ids[i - an.work_count] = i;
        }

        // load?
    }


    static void count_background_animation(BackgroundAnimation& an, MemoryCounts& counts)
    {
        count_span(an.table, counts, 256);

        for (u32 i = 0; i < an.data_count; i++)
        {
            count_view(an.data[i], counts);
        }
    }


    static bool create_background_animation(BackgroundAnimation& an, Memory& memory)
    {
        bool ok = true;

        ok &= create_span(an.table, memory);

        for (u32 i = 0; i < an.data_count; i++)
        {
            ok &= create_view(an.data[i], memory);
        }

        return ok;
    }


    static BackgroundPartPair get_animation_pair(BackgroundAnimation& an, Randomf32& rng, u64 pos)
    {
        BackgroundPartPair bp;

        /*auto W = BACKGROUND_DIMS.proc.width;
        auto H = BACKGROUND_DIMS.proc.height;

        pos <<= an.speed_shift; // speed
        pos %= (4 * H);

        u32 work_1 = pos / H;
        u32 work_2 = (work_1 + 1) & (4 - 1);

        pos %= H;
        
        bp.height2 = pos;
        bp.height1 = H - bp.height2;

        bp.data1 = an.work_data[work_1].data + bp.height2 * W;
        bp.data2 = an.work_data[work_2].data;

        if (work_1 != an.work_next)
        { 
            auto select = next_random_u32(rng, 0, an.select_count - 1);
            auto bg_id = an.select_ids[select];

            an.select_ids[select] = an.work_ids[an.work_next];
            an.work_ids[an.work_next] = bg_id;

            an.work_next = work_1;
        }*/

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