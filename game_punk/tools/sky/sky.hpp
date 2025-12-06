#pragma once

#include "../utils/tools_include.hpp"



/* sky images */

namespace sky
{
    class SkyImageResult
    {
    public:
        static constexpr cstr name = "sky";

        u32 sky_width = 0;
        u32 sky_height = 0;

        static constexpr u32 rect_height = 4;
        Rect2Du32 roi_rect;

        PathList files;
        ImageList<p32> list;

        u32 n_expected = 0;
        u32 n_read = 0;
    };


    static SkyImageResult get_sky_images()
    {
        SkyImageResult result;
        result.n_expected = 0;
        result.n_read = 0;

        auto root = sfs::path(SRC_DIR);

        auto sky_dir = root / "Base";

        result.files = util::get_png_files(sky_dir);
        result.n_expected += result.files.size();        

        result.n_read += util::count_read_image_files_rotate_90(result.files, result.list);

        assert(result.files.size() == result.list.size());

        if (!result.list.empty())
        {
            result.sky_width = result.list[0].width;
            result.sky_height = result.list[0].height;
            result.roi_rect = img::make_rect(result.sky_width, result.rect_height);
        }

        return result;
    }
}


/* process sky images */

namespace sky
{
    static void blend_sky(img::SubView const& src, img::ImageView const& dst)
    {
        assert(dst.height == 1);

        auto w = src.width;
        auto h = src.height;

        f32 r = 0.0f;
        f32 g = 0.0f;
        f32 b = 0.0f;

        p32 ps;

        // first row, blend all rows together
        auto s0 = img::row_begin(src, 0);
        for (u32 x = 0; x < w; x++)
        {
            r = 0.0f;
            g = 0.0f;
            b = 0.0f;

            for (u32 y = 0; y < h; y++)
            {
                ps = *img::xy_at(src, x, y);
                r += ps.red;
                g += ps.green;
                b += ps.blue;
            }

            r /= h;
            g /= h;
            b /= h;

            s0[x] = img::to_pixel((u8)r, (u8)g, (u8)b);
        }

        constexpr u32 bw = 9;

        constexpr u32 x_begin = bw / 2;
        u32 x_end = w - x_begin;

        auto blend_row = [&](u32 y)
        {
            auto rs = img::row_begin(src, y - 1);
            auto rd = img::row_begin(src, y);            

            for (u32 dx = x_begin; dx < x_end; dx++)
            {
                r = 0.0f;
                g = 0.0f;
                b = 0.0f;

                for (u32 i = 0; i < bw; i++)
                {
                    u32 sx = dx - x_begin + i;
                    ps = rs[sx];
                    r += ps.red;
                    g += ps.green;
                    b += ps.blue;
                }

                r /= bw;
                g /= bw;
                b /= bw;

                rd[dx] = img::to_pixel((u8)r, (u8)g, (u8)b);
            }
        };

        blend_row(1);
        blend_row(2);
        blend_row(3);

        // copy fourth row to dst
        auto r3 = img::row_span(src, 3);
        auto rd = img::to_span(dst);
        
        span::copy(r3, rd);
    }

    
    inline u32 count_write_image_sub_view_files(PathList const& files, ImageList<p32>& list, Rect2Du32 rect, cstr dst_dir)
    {
        assert(files.size() == list.size());

        StackBuffer<u8, 128> path_data;
        auto path_sv = span::make_string_view(path_data);

        u32 w = rect.x_end - rect.x_begin;
        u32 h = 1;

        auto buffer = img::create_buffer32(w * h, "sky blend");

        auto blend = img::make_view(w, h, buffer);

        auto write_view = [&](img::ImageView const& view, cstr name)
        {
            img::Image dst;
            dst.data_ = view.matrix_data_;
            dst.width = view.width;
            dst.height = view.height;

            span::zero_string(path_sv);
            span::sprintf(path_sv, "%s/%s", dst_dir, name);

            img::write_image(dst, span::to_cstr(path_sv));
        };

        u32 count = 0;

        for (u32 i = 0; i < files.size(); i++)
        {
            auto& file = files[i];
            auto& src = list[i];

            auto sub = img::sub_view(src, rect);

            blend_sky(sub, blend);
            auto name = file.filename().c_str();

            write_view(blend, name);

            img::destroy_image(src);
            count++;
        }

        mb::destroy_buffer(buffer);

        return count;
    }
}


/* overlay */

namespace sky
{
    class OverlayImageResult
    {
    public:
        static constexpr cstr name = "overlay";

        PathList files;
        ImageList<p32> list;

        u32 n_expected = 0;
        u32 n_read = 0;
    };


    static OverlayImageResult get_overlay_images()
    {
        OverlayImageResult result{};result.n_expected = 0;
        result.n_read = 0;

        auto root = sfs::path(SRC_DIR);

        auto overlay_dir = root / "Overlay";

        result.files = util::get_png_files(overlay_dir);
        result.n_expected += result.files.size();        

        result.n_read += util::count_read_image_files_rotate_90(result.files, result.list);

        assert(result.files.size() == result.list.size());

        return result;
    }
}


/* process overlay images */

namespace sky
{    
    static u32 count_write_convert_images(PathList const& files, ImageList<p32>& list, cstr ov_dir, cstr table_dir)
    {
        StackBuffer<u8, 128> path_data;
        auto path_sv = span::make_string_view(path_data);

        auto write_ov = [&](auto const& dst, cstr name)
        {
            span::zero_string(path_sv);
            span::sprintf(path_sv, "%s/%s", ov_dir, name);

            img::write_image(dst, span::to_cstr(path_sv));
        };

        auto write_table = [&](auto const& dst, cstr name)
        {
            span::zero_string(path_sv);
            span::sprintf(path_sv, "%s/%s", table_dir, name);

            img::write_image(dst, span::to_cstr(path_sv));
        };

        u32 count = 0;

        for (u32 i = 0; i < files.size(); i++)
        {
            auto& file = files[i];
            auto& src = list[i];

            auto table = util::create_color_table_image(src);
            auto gray = util::convert_image(src, table);
            

            auto name = file.filename().c_str();
            
            write_ov(gray, name);
            write_table(table, name);

            img::destroy_image(src);
            img::destroy_image(table);
            img::destroy_image(gray);
            count++;
        }

        return count;
    }
}


namespace sky
{
    static void print_result(auto const& result, u32 n_written)
    {
        printf("%s: %u/%u/%u\n", result.name, result.n_expected, result.n_read, n_written);
    }


    void generate_sky()
    {
        constexpr auto base_dir = OUT_SKY_BASE_DIR;
        constexpr auto ov_dir = OUT_SKY_OVERLAY_DIR;
        constexpr auto table_dir = OUT_SKY_TABLE_DIR;

        sfs::create_directories(base_dir);
        sfs::create_directories(ov_dir);
        sfs::create_directories(table_dir);

        auto res_sky = get_sky_images();
        u32 n_sky = 0;
        n_sky += count_write_image_sub_view_files(res_sky.files, res_sky.list, res_sky.roi_rect, base_dir);

        auto res_ov = get_overlay_images();
        
        u32 n_ov = 0;
        n_ov = count_write_convert_images(res_ov.files, res_ov.list, ov_dir, table_dir);
        
        printf("\n--- sky ---\n");
        print_result(res_sky, n_sky);
        print_result(res_ov, n_ov);
        printf("\n"); 

    }
}