#pragma once

#include "../../../libs/io/filesystem.hpp"
#include "../../../libs/image/image.hpp"
#include "../../../libs/datetime/datetime.hpp"

#include <filesystem>
#include <vector>
#include <array>
#include <set>
#include <cstdio>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>



namespace sfs = std::filesystem;
namespace img = image;
namespace mb = memory_buffer;
namespace dt = datetime;

using p32 = img::Pixel;
using PathList = std::vector<sfs::path>;

template <typename T>
using ImageList = std::vector<Matrix2D<T>>;

using List2Du32 = std::vector<Vec2Du32>;


#ifdef _WIN32
#error Tools runs on the Linux machine only
#endif


namespace sky
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/image/sky";

    constexpr auto OUT_SKY_BASE_DIR    = "/home/adam/Repos/GamePunkRun/game_punk/tools/sky/out_files/base";
    constexpr auto OUT_SKY_OVERLAY_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/sky/out_files/overlay";
    constexpr auto OUT_SKY_TABLE_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/sky/out_files/table";
}


namespace bg
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/image/backgrounds";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/background/out_files/gen";
}


namespace sprite
{
    constexpr auto SRC_CHARACTER_DIR = "/home/adam/Desktop/Game Assets/image/characters";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/sprite/out_files/gen";
}


namespace tile
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/image/tiles";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/tile/out_files/gen";
}


namespace ui
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/image/ui";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/ui/out_files/gen";
}


namespace bin
{
    constexpr auto OUT_BIN_DIR = "/home/adam/Repos/GamePunkRun/game_punk/res/xbin";
    constexpr auto OUT_BIN_FILE = "punk_run.bin";
    constexpr auto OUT_BIN_TABLE_FILE = "bin_table.hpp";

    constexpr auto BIN_TABLE_TYPES_PATH = "/home/adam/Repos/GamePunkRun/game_punk/tools/z_make_bin/bin_table_types.hpp";
    
}


namespace util
{
    inline PathList get_png_files(sfs::path const& dir)
    {
        PathList list;

        if (!sfs::is_directory(dir))
        {
            return list;
        }
        
        for (auto const& entry :sfs::directory_iterator(dir))
        {
            auto& path = entry.path();

            if (!sfs::is_regular_file(entry) || path.extension() != ".png")
            {
                continue;
            }

            list.push_back(path);
        }

        return list;
    }


    inline PathList get_sub_directories(sfs::path const& root)
    {
        PathList list;

        if (!sfs::is_directory(root))
        {
            return list;
        }

        for (auto const& entry :sfs::directory_iterator(root))
        {
            auto& path = entry.path();
            if (sfs::is_directory(path))
            {
                list.push_back(path);
            }
        }

        return list;
    }
    
    
    template <typename P>
    inline u32 count_read_image_files(PathList const& files, ImageList<P>& dst)
    {
        u32 count = 0;

        for (auto& file : files)
        {
            img::Image png;
            if (img::read_image_from_file(file.string().c_str(), png))
            {
                dst.push_back(png);
                count++;
            }
        }

        return count;
    }


    inline u32 count_read_image_files_rotate_90(PathList const& files, ImageList<p32>& dst)
    {
        u32 count = 0;

        for (auto& file : files)
        {
            img::Image png;
            if (!img::read_image_from_file(file.string().c_str(), png))
            {
                continue;                
            }

            img::Image png_90;
            if (!img::create_image(png_90, png.height, png.width))
            {
                img::destroy_image(png);
                continue;
            }

            img::rotate_90(img::make_view(png), img::make_view(png_90));
            img::destroy_image(png);

            dst.push_back(png_90);
            count++;
        }

        return count;
    }


    template <typename P>
    inline u32 count_write_image_files(ImageList<P>& list, cstr dst_dir)
    {
        u32 count = 0;
        int file_id = 0;

        StackBuffer<u8, 128> path_data;
        auto path_sv = span::make_string_view(path_data);

        for (auto& src : list)
        {
            file_id++;
            span::zero_string(path_sv);
            span::sprintf(path_sv, "%s/%02d.png", dst_dir, file_id);

            img::write_image(src, span::to_cstr(path_sv));

            img::destroy_image(src); // Do later?
            count++;
        }

        return count;
    }
    
    
    inline std::string get_varialbe_name(cstr path, cstr prefix)
    {
        StackBuffer<u8, 32> name_data;
        auto name_sv = span::make_string_view(name_data);

        fs::copy_file_name(path, name_sv);

        // replace '.' with '_'
        auto& name = name_sv;
        for (u32 i = 0; i < name.length; i++)
        {
            if (name.data[i] == '.')
            {
                name.data[i] = '_';
            }
        }

        return std::string(prefix) + "_" + span::to_cstr(name_sv);
    }


    inline void write_buffer(MemoryBuffer<u8>& buffer, std::ofstream& bin_file)
    {
        bin_file.write((char*)buffer.data_, buffer.size_);
    }


    inline bool read_image(sfs::path const& path, img::Image& dst)
    {
        return img::read_image_from_file(path.c_str(), dst);
    }


    inline bool write_image(img::Image const& image, sfs::path const& path)
    {
        return img::write_image(image, path.c_str());
    }


    inline bool write_image(img::ImageGray const& image, sfs::path const& path)
    {
        return img::write_image(image, path.c_str());
    }


    inline bool write_image(img::ImageView const& view, sfs::path const& path)
    {
        return img::write_image(img::as_image(view), path.c_str());
    }
}


/* image transforms */

namespace util
{
    static void transform_mask(img::Image const& src, img::ImageGray const& dst)
    {
        auto s = img::to_span(img::make_view(src));
        auto d = img::to_span(img::make_view(dst));

        p32 ps;
        u8 pd;

        for (u32 i = 0; i < s.length; i++)
        {
            ps = s.data[i];
            
            d.data[i] = ps.alpha;
        }
    }
}


/* classes, types */

namespace util
{
    constexpr f32 COEFF_RED = 0.299f;
    constexpr f32 COEFF_GREEN = 0.587f;
    constexpr f32 COEFF_BLUE = 0.114f;


    inline constexpr f32 rgb_to_gray(u8 r, u8 g, u8 b)
    {
        return COEFF_RED * r + COEFF_GREEN * g + COEFF_BLUE * b;
    }


    class TableColor
    {
    public:
        u8 red = 0;
        u8 green = 0;
        u8 blue = 0;
        u8 alpha = 0;
        
        f32 gray = 0.0f;

        TableColor() {};

        TableColor(p32 p)
        {
            red = p.red;
            green = p.green;
            blue = p.blue;
            alpha = p.alpha;
            gray = (p.alpha == 255 ? 1 : -1) * rgb_to_gray(red, green, blue);
        }

        TableColor(u32 n)
        {
            auto p = *((p32*)&n);

            red = p.red;
            green = p.green;
            blue = p.blue;
            alpha = p.alpha;
            gray = (p.alpha == 255 ? 1 : -1) * rgb_to_gray(red, green, blue);
        }

        bool operator < (TableColor const& other) { return gray < other.gray; }
    };
}


namespace util
{
    

    inline img::Image create_color_table_image(img::Image const& src)
    {
        std::set<u32> unique;

        auto const insert = [&](p32 p)
        {
            unique.insert(img::as_u32(p));
        };

        img::for_each_pixel(img::make_view(src), insert);

        std::vector<TableColor> colors(unique.begin(), unique.end());
        std::sort(colors.begin(), colors.end());

        u32 N = colors.size();

        assert(N <= 256);

        img::Image dst;
        dst.data_ = 0;

        if (!img::create_image(dst, N, 1))
        {
            assert("*** Image error - color table ***" && false);
            return dst;
        }

        for (u32 i = 0; i < N; i++)
        {
            auto& color = colors[i];
            dst.data_[i] = img::to_pixel(color.red, color.green, color.blue, color.alpha);
        }

        return dst;
    }


    inline img::Image create_color_table_image(ImageList<p32> const& list)
    {
        std::set<u32> unique;

        auto const insert = [&](p32 p)
        {
            unique.insert(img::as_u32(p));
        };

        for (auto const& item : list)
        {
            img::for_each_pixel(img::make_view(item), insert);
        }

        std::vector<TableColor> colors(unique.begin(), unique.end());
        std::sort(colors.begin(), colors.end());

        u32 N = colors.size();

        assert(N <= 256);

        img::Image dst;
        dst.data_ = 0;

        if (!img::create_image(dst, N, 1))
        {
            assert("*** Image error - color table ***" && false);
            return dst;
        }

        for (u32 i = 0; i < N; i++)
        {
            auto& color = colors[i];
            dst.data_[i] = img::to_pixel(color.red, color.green, color.blue, color.alpha);
        }

        return dst;
    }


    inline img::ImageGray convert_image(img::Image const& src, img::Image const& table)
    {
        img::ImageGray dst;
        dst.data_ = 0;

        if (!img::create_image(dst, src.width, src.height))
        {
            assert("*** Image error - convert image ***" && false);
            return dst;
        }

        auto s = img::to_span(src);
        auto d = img::to_span(dst);
        auto t = img::to_span(table);

        for (u32 i = 0; i < s.length; i++)
        {
            auto ps = img::as_u32(s.data[i]);
            d.data[i] = 0;
            
            for (u32 c = 0; c < 256; c++)
            {
                auto pt = img::as_u32(t.data[c]);
                if (pt == ps)
                {
                    d.data[i] = (u8)c;
                    break;
                }
            }
        }

        return dst;
    }
}