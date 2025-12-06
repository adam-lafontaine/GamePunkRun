#pragma once

#include "../utils/tools_include.hpp"


namespace ui
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


    static ImageResult get_images(sfs::path const& dir)
    {
        ImageResult result;

        result.name = dir.filename();
        result.n_expected = 0;
        result.n_read = 0;

        result.files = util::get_png_files(dir);
        result.n_expected = result.files.size();

        util::count_read_image_files_rotate_90(result.files, result.images);
        result.n_read = result.images.size();

        assert(result.n_expected == result.n_read);

        return result;
    }
}


/* font images */

namespace ui
{
    class FontImageResult
    {
    public:
        std::string name;

        PathList digit_files;
        PathList alpha_files_1;
        PathList alpha_files_2;

        ImageList<p32> digit_images;
        ImageList<p32> alpha_images_1;
        ImageList<p32> alpha_images_2;

        u32 n_expected = 0;
        u32 n_read = 0;
    };


    static FontImageResult get_font_images(sfs::path const& dir)
    {
        FontImageResult result;

        result.name = dir.filename();
        result.n_expected = 0;
        result.n_read = 0;

        for (auto const& file : util::get_png_files(dir))
        {
            auto name = file.filename().string();
            if (std::isdigit(name[0]))
            {
                result.digit_files.push_back(file);
            }
            else if (isalpha(name[0]))
            {
                switch (name[1])
                {
                case '1':
                    result.alpha_files_1.push_back(file);
                    break;

                case '2':
                    result.alpha_files_2.push_back(file);
                    break;

                default: break;
                }
            }
        }

        result.n_expected = result.digit_files.size() + result.alpha_files_1.size() + result.alpha_files_2.size();

        auto sort = [](PathList& list) 
        { 
            auto comp = [](auto const& a, auto const& b) { return a.filename() < b.filename(); };
            std::sort(list.begin(), list.end(), comp); 
        };

        sort(result.digit_files);
        sort(result.alpha_files_1);
        sort(result.alpha_files_2);

        result.n_read += util::count_read_image_files_rotate_90(result.digit_files, result.digit_images);
        result.n_read += util::count_read_image_files_rotate_90(result.alpha_files_1, result.alpha_images_1);
        result.n_read += util::count_read_image_files_rotate_90(result.alpha_files_2, result.alpha_images_2);

        assert(result.n_expected == result.n_read);

        return result;
    }
}


namespace ui
{    
    static u32 count_write_font_mask_image(FontImageResult& res, sfs::path const& out_dir)
    {
        u32 count = 0;

        u32 width = 16; 

        u32 h = 0;

        u32 n = 0;
        for (auto const& src : res.digit_images)
        {
            h = src.height > h ? src.height : h;
            n++;
        }

        for (auto const& src : res.alpha_images_1)
        {
            h = src.height > h ? src.height : h;
            n++;
        }

        for (auto const& src : res.alpha_images_2)
        {
            h = src.height > h ? src.height : h;
            n++;
        }

        u32 height = h * n;

        img::Image dst;
        img::ImageGray mask;

        bool ok = true;

        ok &= img::create_image(dst, width, height);
        ok &= img::create_image(mask, width, height);

        if (!ok)
        {
            img::destroy_image(dst);
            img::destroy_image(mask);
        }

        img::fill(img::make_view(dst), img::to_pixel(0, 0, 0, 0));

        u32 y = 0;
        u32 x = 0;
        auto dr = img::make_rect(width, 0);

        auto const add_images = [&](auto& list)
        {
            for (auto& src : list)
            {
                x = (width - src.width) / 2;
                dr = img::make_rect(x, y, src.width, src.height);
                img::copy(img::make_view(src), img::sub_view(dst, dr));
                img::destroy_image(src);
                y += h;
            }            
        };
        
        add_images(res.digit_images);
        add_images(res.alpha_images_1);
        add_images(res.alpha_images_2);

        util::transform_mask(dst, mask);

        count += util::write_image(mask, (out_dir / "font.png"));

        img::destroy_image(dst);
        img::destroy_image(mask);

        return count;
    }


    static u32 count_write_convert_image_files(ImageResult& res, img::Image const& table, sfs::path const& dst_dir)
    {
        u32 count = 0;

        auto N = res.files.size();

        for (u32 i = 0; i < N; i++)
        {
            auto& file = res.files[i];
            auto& src = res.images[i];

            auto gray = util::convert_image(src, table);

            auto path = dst_dir / file.filename();
            util::write_image(gray, path);

            img::destroy_image(src);
            img::destroy_image(gray);
            count++;
        }

        return count;
    }
}


namespace ui
{
    static void print_result(auto const& result, u32 n_written)
    {
        printf("%s: %u/%u/%u\n", result.name.c_str(), result.n_expected, result.n_read, n_written);
    }


    void generate_ui()
    {
        auto out_dir = sfs::path(OUT_DIR);
        sfs::create_directories(out_dir);

        auto in_dir = sfs::path(SRC_DIR);

        printf("\n--- ui ---\n");

        for (auto const& dir : util::get_sub_directories(in_dir))
        {
            auto out = out_dir / dir.filename();
            sfs::create_directories(out);

            // Magic!
            auto out_files = out / "images";
            sfs::create_directories(out_files);

            if (dir.filename() == "Font")
            {
                auto res = get_font_images(dir);
                auto n_font = count_write_font_mask_image(res, out_files);
                print_result(res, n_font);

                auto res_table = get_images(dir);
                auto table = util::create_color_table_image(res_table.images);

                auto path = out / "table.png";
                util::write_image(table, path);
                img::destroy_image(table);

                for (auto& src :res_table.images)
                {
                    img::destroy_image(src);
                }
            }
            else
            {
                auto res = get_images(dir);
                auto table = util::create_color_table_image(res.images);

                auto path = out / "table.png";
                util::write_image(table, path);

                auto n_image = count_write_convert_image_files(res, table, out_files);

                print_result(res, n_image);
                img::destroy_image(table);
                
            }


        }
    }
}