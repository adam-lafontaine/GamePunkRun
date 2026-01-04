// stuff I may want to keep

/* image view pma */

namespace game_punk
{
    class ViewRGBAf32
    {
    public:
        u32 width = 0;
        u32 height = 0;

        struct 
        {
            f32* red = 0;
            f32* green = 0;
            f32* blue = 0;
            f32* alpha = 0;

        } data;
    };


    class ViewRGBf32
    {
    public:
        u32 width = 0;
        u32 height = 0;

        struct 
        {
            f32* red = 0;
            f32* green = 0;
            f32* blue = 0;

        } data;
    };


    p32 to_pixel(f32 red, f32 green, f32 blue)
    {
        constexpr f32 u8_max = 255.0f;

        auto r = math::cxpr::round_to_unsigned<u8>(u8_max * red);
        auto g = math::cxpr::round_to_unsigned<u8>(u8_max * green);
        auto b = math::cxpr::round_to_unsigned<u8>(u8_max * blue);

        return img::to_pixel(r, g, b);
    }


    void convert_view(Image const& src, ViewRGBf32 const& dst)
    {
        app_assert(src.width == dst.width);
        app_assert(src.height == dst.height);
        app_assert(src.data_);
        app_assert(dst.data.red);

        constexpr f32 scale = 1.0f / 255.0f;

        auto w = src.width;
        auto h = src.height;

        auto len = w * h;

        auto red = dst.data.red;
        auto green = dst.data.green;
        auto blue = dst.data.blue;

        for (u32 i = 0; i < len; i++)
        {
            auto p = src.data_[i];

            red[i] = p.red * scale;
            green[i] = p.green * scale;
            blue[i] = p.blue * scale;
        }
    }


    void convert_view(Image const& src, ViewRGBAf32 const& dst)
    {
        app_assert(src.width == dst.width);
        app_assert(src.height == dst.height);
        app_assert(src.data_);
        app_assert(dst.data.red);

        constexpr f32 scale = 1.0f / 255.0f;

        auto w = src.width;
        auto h = src.height;

        auto len = w * h;

        auto red = dst.data.red;
        auto green = dst.data.green;
        auto blue = dst.data.blue;
        auto alpha = dst.data.alpha;

        for (u32 i = 0; i < len; i++)
        {
            auto p = src.data_[i];

            red[i] = p.red * scale;
            green[i] = p.green * scale;
            blue[i] = p.blue * scale;
            alpha[i] = p.alpha * scale;
        }
    }


    void convert_view_pma(Image const& src, ViewRGBAf32 const& dst)
    {
        app_assert(src.width == dst.width);
        app_assert(src.height == dst.height);
        app_assert(src.data_);
        app_assert(dst.data.red);

        constexpr f32 scale = 1.0f / 255.0f;

        auto w = src.width;
        auto h = src.height;

        auto len = w * h;

        auto red = dst.data.red;
        auto green = dst.data.green;
        auto blue = dst.data.blue;
        auto alpha = dst.data.alpha;

        for (u32 i = 0; i < len; i++)
        {
            auto p = src.data_[i];

            alpha[i] = p.alpha * scale;

            red[i] = p.red * scale * alpha[i];
            green[i] = p.green * scale * alpha[i];
            blue[i] = p.blue * scale * alpha[i];            
        }
    }


    void convert_view(ViewRGBf32 const& src, ImageView const& dst)
    {
        app_assert(src.width == dst.width);
        app_assert(src.height == dst.height);
        app_assert(dst.matrix_data_);
        app_assert(src.data.red);

        auto w = src.width;
        auto h = src.height;

        auto len = w * h;

        app_assert(len % 4 == 0);

        auto red = src.data.red;
        auto green = src.data.green;
        auto blue = src.data.blue;

        for (u32 i = 0; i < len; i += 4)
        {
            dst.matrix_data_[i + 0] = to_pixel(red[i + 0], green[i + 0], blue[i + 0]);
            dst.matrix_data_[i + 1] = to_pixel(red[i + 1], green[i + 1], blue[i + 1]);
            dst.matrix_data_[i + 2] = to_pixel(red[i + 2], green[i + 2], blue[i + 2]);
            dst.matrix_data_[i + 3] = to_pixel(red[i + 3], green[i + 3], blue[i + 3]);
        }
    }


    void copy_view(ViewRGBf32 const& src, ViewRGBf32 const& dst)
    {
        app_assert(src.width == dst.width);
        app_assert(src.height == dst.height);
        app_assert(src.data.red);
        app_assert(dst.data.red);

        auto len = src.width * src.height * 3;

        span::copy(span::make_view(src.data.red, len), span::make_view(dst.data.red, len));        
    }
    
    
    void blend_view_pma(ViewRGBAf32 const& src, ViewRGBf32 const& dst)
    {
        app_assert(src.width == dst.width);
        app_assert(src.height == dst.height);
        app_assert(src.data.red);
        app_assert(dst.data.red);

        auto w = src.width;
        auto h = src.height;

        auto len = w * h;

        auto sr = src.data.red;
        auto sg = src.data.green;
        auto sb = src.data.blue;
        auto sa = src.data.alpha;

        auto dr = dst.data.red;
        auto dg = dst.data.green;
        auto db = dst.data.blue;

        for (u32 i = 0; i < len; i++)
        {
            auto ia = 1.0f - sa[i];
            dr[i] = sr[i] + ia * dr[i];
            dg[i] = sg[i] + ia * dg[i];
            db[i] = sb[i] + ia * db[i];
        }
    }


    void blend_view_pma(ViewRGBAf32 const& src, ViewRGBAf32 const& dst)
    {
        app_assert(src.width == dst.width);
        app_assert(src.height == dst.height);
        app_assert(src.data.red);
        app_assert(dst.data.red);

        auto w = src.width;
        auto h = src.height;

        auto len = w * h;

        auto sr = src.data.red;
        auto sg = src.data.green;
        auto sb = src.data.blue;
        auto sa = src.data.alpha;

        auto dr = dst.data.red;
        auto dg = dst.data.green;
        auto db = dst.data.blue;

        for (u32 i = 0; i < len; i++)
        {
            auto ia = 1.0f - sa[i];
            dr[i] = sr[i] + ia * dr[i];
            dg[i] = sg[i] + ia * dg[i];
            db[i] = sb[i] + ia * db[i];
        }
    }
}

namespace game_punk
{
    void fill_view(ViewRGBf32 const& view, p32 color)
    {
        app_assert(view.width);
        app_assert(view.height);
        app_assert(view.data.red);

        constexpr f32 scale = 1.0f / 255.0f;

        auto r = color.red * scale;
        auto g = color.green * scale;
        auto b = color.blue * scale;

        auto w = view.width;
        auto h = view.height;

        auto len = w * h;

        auto red = view.data.red;
        auto green = view.data.green;
        auto blue = view.data.blue;

        for (u32 i = 0; i < len; i++)
        {
            red[i] = r;
            green[i] = g;
            blue[i] = b;
        }
    }
}


#ifdef GAME_PUNK_SIMD

#include <immintrin.h>

/* blend simd */

namespace game_punk
{
    void blend_view_pma_simd(ViewRGBAf32 const& src, ViewRGBf32 const& dst)
    {
        app_assert(src.width == dst.width);
        app_assert(src.height == dst.height);
        app_assert(src.data.red);
        app_assert(dst.data.red);

        auto w = src.width;
        auto h = src.height;

        auto len = w * h;

        app_assert(len % 4 == 0);

        auto sr = src.data.red;
        auto sg = src.data.green;
        auto sb = src.data.blue;
        auto sa = src.data.alpha;

        auto dr = dst.data.red;
        auto dg = dst.data.green;
        auto db = dst.data.blue;

        auto vsr = _mm_load_ps(sr);
        auto vsg = _mm_load_ps(sg);
        auto vsb = _mm_load_ps(sb);
        auto vsa = _mm_load_ps(sa);

        auto vdr = _mm_load_ps(dr);
        auto vdg = _mm_load_ps(dg);
        auto vdb = _mm_load_ps(db);

        auto one = 1.0f;
        auto vone = _mm_broadcast_ss(&one);

        auto via = _mm_sub_ps(vone, vsa);

        for (u32 i = 0; i < len; i += 4)
        {
            vsr = _mm_load_ps(sr + i);
            vsg = _mm_load_ps(sg + i);
            vsb = _mm_load_ps(sb + i);
            vsa = _mm_load_ps(sa + i);

            vdr = _mm_load_ps(dr + i);
            vdg = _mm_load_ps(dg + i);
            vdb = _mm_load_ps(db + i);

            via = _mm_sub_ps(vone, vsa);

            vdr = _mm_fmadd_ps(via, vdr, vsr);
            vdg = _mm_fmadd_ps(via, vdg, vsg);
            vdb = _mm_fmadd_ps(via, vdb, vsb);

            _mm_store_ps(dr + i, vdr);
            _mm_store_ps(dg + i, vdg);
            _mm_store_ps(db + i, vdb);
        }
    }
}
#endif


namespace game_punk
{
    static void count_view(ViewRGBAf32& view, MemoryCounts& counts, u32 width, u32 height)
    {
        view.width = width;
        view.height = height;

        auto n_elements = width * height * 4;

        add_count<f32>(counts, n_elements);
    }


    static bool create_view(ViewRGBAf32& view, Memory& mem)
    {
        if (!view.width || !view.height)
        {
            app_assert("*** ViewRGBAf32 not initialized ***" && false);
            return false;
        }

        auto len = view.width * view.height;

        auto n_elements = len * 4;

        auto res = push_mem<f32>(mem, n_elements);
        if (res.ok)
        {
            view.data.red = res.data;
            view.data.green = view.data.red + len;
            view.data.blue = view.data.green + len;
            view.data.alpha = view.data.blue + len;
        }

        return res.ok;
    }


    static void count_view(ViewRGBf32& view, MemoryCounts& counts, u32 width, u32 height)
    {
        view.width = width;
        view.height = height;

        auto n_elements = width * height * 3;

        add_count<f32>(counts, n_elements);
    }


    static bool create_view(ViewRGBf32& view, Memory& mem)
    {
        if (!view.width || !view.height)
        {
            app_assert("*** ViewRGBf32 not initialized ***" && false);
            return false;
        }

        auto len = view.width * view.height;

        auto n_elements = len * 3;

        auto res = push_mem<f32>(mem, n_elements);
        if (res.ok)
        {
            view.data.red = res.data;
            view.data.green = view.data.red + len;
            view.data.blue = view.data.green + len;
        }

        return res.ok;
    }
}


namespace game_punk
{
    static void alpha_blend_span(SpanView<Pixel> const& src, SpanView<Pixel> const& dst)
    {
        constexpr auto scale = 1.0f / 255.0f;
        constexpr auto P = sizeof(Pixel);

        auto s = src.data;
        auto sr = &(s->red);
        auto sg = &(s->green);
        auto sb = &(s->blue);
        auto sa = &(s->alpha);

        auto d = dst.data;
        auto dr = &(d->red);
        auto dg = &(d->green);
        auto db = &(d->blue);

        f32 a = 0.0f;
        f32 ia = 0.0f;

        f32 r_pma = 0.0f;
        f32 g_pma = 0.0f;
        f32 b_pma = 0.0f;

        f32 r = 0.0f;
        f32 g = 0.0f;
        f32 b = 0.0f;        

        sr -= P;
        sg -= P;
        sb -= P;
        sa -= P;
        dr -= P;
        dg -= P;
        db -= P;

        for (u32 i = 0; i < dst.length; ++i)
        {
            //alpha_blend(src.data + i, dst.data + i);            

            sr += P;
            sg += P;
            sb += P;
            sa += P;
            dr += P;
            dg += P;
            db += P;

            auto alpha = *sa;

            switch (alpha)
            {
            case 0:
                continue;
                break;

            case 126:
            case 127:
            case 128:
            case 129:
                a = 0.5f;
                break;

            default:
                a = alpha * scale;
                break;
            }

            //a = alpha * scale;
            
            ia = 1.0f - a;

            r_pma = a * (*sr) + 0.5f;
            g_pma = a * (*sg) + 0.5f;
            b_pma = a * (*sb) + 0.5f;

            r = ia * (*dr) + r_pma;
            g = ia * (*dg) + g_pma;
            b = ia * (*db) + b_pma;

            (*dr) = (u8)r;
            (*dg) = (u8)g;
            (*db) = (u8)b;
        }
    }
}


namespace game_punk
{
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


namespace game_punk
{
    

    
    class BackgroundAnimationFast
    {
    public:

        u32 count = 0;
        u32 speed_shift = 0;

        BackgroundView background_data[cxpr::BACKGROUND_COUNT_MAX]; // Fast, uses more memory

        u8 work_ids[4] = {0};
        u8 select_ids[cxpr::BACKGROUND_COUNT_MAX - 4] = {0};        

        u32 work_next = 0;
    };


    static void reset_background_animation(BackgroundAnimationFast& an)
    {
        bool ok = has_data(an.background_data[0]);

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
            count_view(an.background_data[i], counts);
        }        
    }


    static bool create_background_animation(BackgroundAnimationFast& an, Memory& memory)
    {
        bool ok = true;

        for (u32 i = 0; i < an.count; i++)
        {
            ok &= create_view(an.background_data[i], memory);
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

        bp.data1 = an.background_data[an.work_ids[work_1]].data + bp.height2 * W;
        bp.data2 = an.background_data[an.work_ids[work_2]].data;

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