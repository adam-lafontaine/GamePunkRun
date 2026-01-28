#pragma once


/* sky animation */

namespace game_punk
{
    class SkyAnimation
    {
    public:
        SkyOverlayView overlay_src;

        ScenePosition ov_pos;
        Vec2Di32 ov_vel;

        BackgroundView out[2];
        u8 out_id = 0;

        BackgroundView& out_front() { return out[out_id]; }

        BackgroundView& out_back() { return out[!out_id]; }

        void out_swap() { out_id = !out_id; }
    };


    static void count_sky_animation(SkyAnimation& sky, MemoryCounts& counts)
    {
        count_view(sky.overlay_src, counts);
        count_view(sky.out[0], counts);
        count_view(sky.out[1], counts);
    }


    static bool create_sky_animation(SkyAnimation& sky, Memory& memory)
    {
        bool ok = true;
        
        ok &= create_view(sky.overlay_src, memory);
        ok &= create_view(sky.out[0], memory);
        ok &= create_view(sky.out[1], memory);

        return ok;
    }
    

    static void render_front_back(SkyAnimation& sky)
    {
        auto front = to_span_cx(sky.out_front());
        auto back = to_span_cx(sky.out_back());

        img::copy(sub_view(sky.overlay_src, sky.ov_pos), to_image_view(sky.out_front()));
        copy(front, back);
    }


    static void reset_sky_animation(SkyAnimation& sky)
    {
        auto vs = make_vec_scene(0, 0);
        sky.ov_pos = ScenePosition(vs, DimCtx::Proc);
        sky.ov_vel = { 4, 2 };

        bool ok = true;
        ok &= has_data(sky.overlay_src);
        ok &= has_data(sky.out[0]);
        ok &= has_data(sky.out[1]);

        app_assert(ok && "*** SkyAnimation not created ***");

        render_front_back(sky);
    }


    static void update_overlay_position(SkyAnimation& sky)
    {
        auto ov = to_image_view(sky.out_back());
        auto w = ov.width;
        auto h = ov.height;

        auto& data_ov = sky.overlay_src;

        i32 xm = data_ov.width - w - 1;
        i32 ym = data_ov.height - h - 1;

        auto& pos = sky.ov_pos.proc;
        auto& vel = sky.ov_vel;

        i32 x = pos.x.get() + vel.x;
        i32 y = pos.y.get() + vel.y;

        if (x < 0 || x > xm)
        {
            vel.x *= -1;
        }
        else
        {
            pos.x = units::SceneDimension::make(x);
        }

        if (y < 0 || y > ym)
        {
            vel.y *= -1;
        }
        else
        {
            pos.y = units::SceneDimension::make(y);
        }

        img::copy(sub_view(data_ov, sky.ov_pos), ov);
    }
    
    
    static BackgroundView get_sky_animation(SkyAnimation& sky, GameTick64 game_tick)
    {
        constexpr u32 frame_wait = 6;

        if (game_tick.value_ % frame_wait == 0)
        {
            update_overlay_position(sky);
            sky.out_swap();
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

        using FilterTable = ObjectTable<BackgroundFilterView>;
        using AssetID = FilterTable::ID;

        FilterTable background_filters;
        BackgroundView background_data[2] = { 0 };
        
        u32 speed_shift = 0;
        p32 primary_color;

        u64 load_pos = 0;
        AssetID current_background;

        RingStackBuffer<AssetID, 4> work_asset_ids;
        RandomStackBuffer<AssetID, cxpr::BACKGROUND_COUNT_MAX - 4> select_asset_ids;
    };


    static void reset_background_animation(BackgroundAnimation& an)
    {
        using AssetID = BackgroundAnimation::AssetID;

        bool ok = true;
        ok &= has_data(an.background_data[0]);
        ok &= has_data(an.background_data[1]);

        app_assert(ok && "*** BackgroundAnimation not created ***");

        an.speed_shift = 0;
        an.load_pos = 0;

        auto WC = an.work_asset_ids.count;
        auto SC = an.select_asset_ids.capacity;

        an.work_asset_ids.cursor.reset();
        
        for (u32 i = 0; i < WC; i++)
        {
            an.work_asset_ids.data[i] = {(u16)i};
        }

        for (u32 i = 0; i < SC; i++)
        {
            an.select_asset_ids.data[i] = { (u16)(i + WC) };
        }
    }


    static void count_background_animation(BackgroundAnimation& an, MemoryCounts& counts, u32 n_backgrounds)
    {
        count_view(an.background_data[0], counts);
        count_view(an.background_data[1], counts);
        count_table(an.background_filters, counts, n_backgrounds);

        BackgroundFilterView filter;
        for (u32 i = 0; i < n_backgrounds; i++)
        {
            count_view(filter, counts);
        }
    }


    static bool create_background_animation(BackgroundAnimation& an, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(an.background_data[0], memory);
        ok &= create_view(an.background_data[1], memory);

        auto& filters = an.background_filters;

        ok &= create_table(filters, memory);
        for (u32 i = 0; i < filters.capacity; i++)
        {
            auto id = filters.push();
            ok &= create_view(filters.item_at(id), memory);
        }

        return ok;
    }


    static BackgroundPartPair get_animation_pair(BackgroundAnimation& an, Randomf32& rng, u64 pos)
    {
        using AssetID = BackgroundAnimation::AssetID;

        BackgroundPartPair bp;

        auto W = BACKGROUND_DIMS.proc.width;
        auto H = BACKGROUND_DIMS.proc.height;

        auto p = pos >> an.speed_shift; // speed
        p %= (2 * H);

        u32 data_1 = p / H;
        u32 data_2 = !data_1;

        p %= H;
        
        bp.height2 = p;
        bp.height1 = H - bp.height2;

        bp.data1 = an.background_data[data_1].data + bp.height2 * W;
        bp.data2 = an.background_data[data_2].data;

        if (bp.height2 == 0 && pos != an.load_pos)
        { 
            an.load_pos = pos;

            // select next background to load
            an.current_background = an.select_asset_ids.get(rng);

            // swap selected id with working id
            auto& work_id = an.work_asset_ids.front();
            an.select_asset_ids.set(work_id);
            work_id = an.current_background;
            an.work_asset_ids.next();

            auto src = to_span(an.background_filters.item_at(an.current_background));
            auto dst = to_span(an.background_data[data_2]);
            bt::alpha_filter_convert(src, dst, an.primary_color);
        }

        return bp;
    }
}


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
        
        count_background_animation(bg.bg_1, counts, bt::Background_Bg1::count);
        count_background_animation(bg.bg_2, counts, bt::Background_Bg2::count);
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