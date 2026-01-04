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

        sfs::path palette_file;
        img::Image palette_image;

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

            if (name == "Palette.png")
            {
                result.n_expected++;
                result.palette_file = file;
            }
            else if (std::isdigit(name[0]))
            {
                result.digit_files.push_back(file);
                result.n_expected++;
            }
            else if (isalpha(name[0]))
            {
                switch (name[1])
                {
                case '1':
                    result.alpha_files_1.push_back(file);
                    result.n_expected++;
                    break;

                case '2':
                    result.alpha_files_2.push_back(file);
                    result.n_expected++;
                    break;

                default: break;
                }
            }
        }

        result.n_read += util::read_image(result.palette_file, result.palette_image);

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


    static u32 count_write_font_table_image(FontImageResult& res, sfs::path const& out)
    {
        u32 count = 0;

        auto table = util::generate_color_table(res.palette_image);
        if (table.rgba.data_)
        {
            count++;
            auto path = out / "table.png";
            util::write_color_table(table, path);
            table.destroy();
        }        
        
        img::destroy_image(res.palette_image);

        return count;
    }


    static u32 count_write_font_mask_image(FontImageResult& res, sfs::path const& out)
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
        AlphaFilterImage mask;

        bool ok = true;

        ok &= img::create_image(dst, width, height);
        ok &= util::create_image(mask, width, height);

        if (!ok)
        {
            img::destroy_image(dst);
            mask.destroy();
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

        count += util::write_image(mask, (out / "font.png"));

        img::destroy_image(dst);
        mask.destroy();

        return count;
    }
}


/* icons */

namespace ui
{
    class IconImageResult
    {
    public:
        std::string name;

        sfs::path palette_file;
        img::Image palette_image;

        sfs::path frame_file;
        img::Image frame_image;

        PathList icon_files;
        ImageList<p32> icon_images;

        u32 n_expected = 0;
        u32 n_read = 0;
    };


    static IconImageResult get_icon_images(sfs::path const& dir)
    {
        IconImageResult result;

        result.name = dir.filename();
        result.n_expected = 0;
        result.n_read = 0;

        for (auto const& file : util::get_png_files(dir))
        {
            auto name = file.filename().string();

            if (name == "Palette.png")
            {
                result.n_expected++;
                result.palette_file = file;
            }
            else if (name[0] == 'F') // Frame
            {
                result.n_expected++;
                result.frame_file = file;
            }
            else if (name[0] == 'S') // SkillIcon
            {
                result.n_expected++;
                result.icon_files.push_back(file);
            }
        }

        result.n_read += util::read_image(result.palette_file, result.palette_image);
        result.n_read += util::read_image(result.frame_file, result.frame_image);

        auto sort = [](PathList& list) 
        { 
            auto comp = [](auto const& a, auto const& b) { return a.filename() < b.filename(); };
            std::sort(list.begin(), list.end(), comp); 
        };

        sort(result.icon_files);

        result.n_read += util::count_read_image_files_rotate_90(result.icon_files, result.icon_images);

        assert(result.n_expected == result.n_read);

        return result;
    }


    static u32 count_write_icon_table_image(IconImageResult& res, sfs::path const& out)
    {
        u32 count = 0;

        auto table = util::generate_color_table(res.palette_image);
        if (table.rgba.data_)
        {
            count++;
            auto path = out / "table.png";
            util::write_color_table(table, path);
            table.destroy();
        }        
        
        img::destroy_image(res.palette_image);

        return count;
    }


    static u32 count_write_icon_filter_image(IconImageResult& res, sfs::path const& out)
    {
        u32 count = 0;

        auto w = res.frame_image.width;
        auto h = res.frame_image.height;

        auto width = w;
        auto height = h * (1 + res.icon_images.size());

        img::Image dst;
        AlphaFilterImage mask;

        bool ok = true;
        ok &= img::create_image(dst, width, height);
        ok &= util::create_image(mask, width, height);

        if (!ok)
        {
            img::destroy_image(dst);
            mask.destroy();

            return 0;
        }

        img::fill(img::make_view(dst), img::to_pixel(0, 0, 0, 0));

        auto r = img::make_rect(w, h);

        img::copy(img::make_view(res.frame_image), img::sub_view(dst, r));
        img::destroy_image(res.frame_image);
        
        for (auto& src : res.icon_images)
        {
            r.y_begin += h;
            r.y_end += h;

            img::copy(img::make_view(src), img::sub_view(dst, r));
            img::destroy_image(src);
        }

        auto primary = img::to_pixel(255, 129, 66); // Magic! from the file
        auto secondary = img::to_pixel(0);

        util::transform_filter(dst, mask, primary, secondary);

        count += util::write_image(mask, (out / "icons.png"));

        img::destroy_image(dst);
        mask.destroy();

        return count;
    }
}


namespace ui
{
    static void print_result(auto const& result, u32 n_written)
    {
        printf("%s: %u/%u/%u\n", result.name.c_str(), result.n_expected, result.n_read, n_written);
    }


    static void generate_font_images(sfs::path const& dir, sfs::path const& out)
    {
        // Magic!
        auto out_files = out / "images";
        sfs::create_directories(out_files);

        auto res = get_font_images(dir);
        auto n_font = count_write_font_mask_image(res, out_files);
        n_font += count_write_font_table_image(res, out);
        print_result(res, n_font);
    }


    static void generate_title_images(sfs::path const& dir, sfs::path const& out)
    {
        // Magic!
        auto out_files = out / "images";
        sfs::create_directories(out_files);

        auto res = get_images(dir);
        auto table = util::generate_color_table(res.images);

        auto path = out / "table.png";
        util::write_color_table(table, path);

        auto n_image = util::count_write_convert_image_files(res.files, res.images, table, out_files);

        print_result(res, n_image);
        table.destroy();
    }


    static void generate_icon_images(sfs::path const& dir, sfs::path const& out)
    {
        // Magic!
        auto out_files = out / "images";
        sfs::create_directories(out_files);

        auto res = get_icon_images(dir);
        auto n_icon = count_write_icon_filter_image(res, out_files);
        n_icon += count_write_icon_table_image(res, out);
        print_result(res, n_icon);
    }
}


namespace ui
{
    


    void generate_ui()
    {
        auto out_dir = sfs::path(OUT_DIR);
        sfs::create_directories(out_dir);

        auto in_dir = sfs::path(SRC_DIR);

        printf("\n--- ui ---\n");

        for (auto const& dir : util::get_sub_directories(in_dir))
        {
            auto name = dir.filename();
            auto out = out_dir / name;            

            if (name == "Font")
            {
                sfs::create_directories(out);
                generate_font_images(dir, out);
            }
            else if (name == "Title")
            {
                sfs::create_directories(out);
                generate_title_images(dir, out);               
            }
            else if (name == "Icons")
            {
                sfs::create_directories(out);
                generate_icon_images(dir, out);
            }
        }
    }
}