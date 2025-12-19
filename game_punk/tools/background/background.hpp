#pragma once

#include "../utils/tools_include.hpp"

/* color palette images */

namespace bg
{
    class ImageResult
    {
    public:
        std::string name;
        PathList files;
        ImageList<p32> images;

        u32 n_expected = 0;
        u32 n_read = 0;
    };


    static ImageResult get_palette_images(sfs::path const& dir)
    {
        ImageResult result;

        result.name = dir.filename();
        result.n_expected = 0;
        result.n_read = 0;

        result.files = util::get_png_files(dir);
        result.n_expected = result.files.size();

        util::count_read_image_files(result.files, result.images);
        result.n_read = result.images.size();

        assert(result.n_expected == result.n_read);

        return result;
    }


    static u32 count_write_palette_image(ImageResult const& res, sfs::path const& out_dir)
    {
        auto table = util::generate_color_table(res.images);

        auto path = out_dir / "table.png";

        auto ok = util::write_color_table(table, path);

        u32 count = ok;

        util::destroy_color_table(table);

        return count;
    }
}

/* background mask images */

namespace bg
{
    static ImageResult get_background_images(sfs::path const& dir)
    {    
        ImageResult result{};
        result.name = dir.filename();
        result.n_expected = 0;
        result.n_read = 0;
        
        result.files = util::get_png_files(dir);
        result.n_expected = result.files.size();
        result.n_read = util::count_read_image_files_rotate_90(result.files, result.images);

        return result;
    }


    static u32 count_write_mask_images(ImageResult& res, sfs::path const& out_dir)
    {
        u32 count = 0;

        u32 N = res.files.size();
        MaskImage dst;

        for (u32 i = 0; i < N; i++)
        {
            auto& file = res.files[i];
            auto& src = res.images[i];

            if (!util::create_image(dst, src.width, src.height))
            {
                img::destroy_image(src);
                continue;
            }

            util::transform_mask(src, dst);

            auto path = out_dir / file.filename();
            bool ok = util::write_image(dst, path);

            util::destroy_image(dst);
            img::destroy_image(src);
            count += ok;
        }

        return count;
    }
}


namespace bg
{
    
    static void print_result(auto const& result, u32 n_written)
    {
        printf("%s: %u/%u/%u\n", result.name.c_str(), result.n_expected, result.n_read, n_written);
    }


    static void generate_backgrounds()
    {        
        auto out_dir = sfs::path(OUT_DIR);
        sfs::create_directories(out_dir);

        auto in_dir = sfs::path(SRC_DIR);

        printf("\n--- backgrounds ---\n");        

        for (auto const& dir : util::get_sub_directories(in_dir))
        {
            auto out = out_dir / dir.filename();
            sfs::create_directories(out);

            if (dir.filename() == "colors") // Magic!
            {
                auto res_pt = get_palette_images(dir);
                auto n_pt = count_write_palette_image(res_pt, out);
                print_result(res_pt, n_pt);
                continue;
            }

            auto res_bg = get_background_images(dir);
            auto n_bg = count_write_mask_images(res_bg, out);
            print_result(res_bg, n_bg);
        }
        
        printf("\n"); 
    }
}