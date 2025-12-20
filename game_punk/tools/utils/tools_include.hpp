#pragma once

#include "../../../libs/io/filesystem.hpp"
#include "../../../libs/image/image.hpp"
#include "../../../libs/datetime/datetime.hpp"
#include "bin_table_types.hpp"

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


#ifdef _WIN32
#error Tools runs on the Linux machine only
#endif


namespace sfs = std::filesystem;
namespace img = image;
namespace mb = memory_buffer;
namespace dt = datetime;

using p32 = img::Pixel;
using PathList = std::vector<sfs::path>;

template <typename T>
using ImageList = std::vector<Matrix2D<T>>;

using List2Du32 = std::vector<Vec2Du32>;


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


namespace icon
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/game_icon";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/res/icon";
}


namespace bin
{
    constexpr auto OUT_BIN_DIR = "/home/adam/Repos/GamePunkRun/game_punk/res/xbin";
    constexpr auto OUT_BIN_FILE = "punk_run.bin";
    constexpr auto OUT_BIN_TABLE_FILE = "bin_table.hpp";

    constexpr auto BIN_TABLE_TYPES_PATH = "/home/adam/Repos/GamePunkRun/game_punk/tools/utils/bin_table_types.hpp";
    
}


using FilterKey = bin_table::FilterKey;
using MaskImage = bin_table::MaskImage1C;
using FilterImage = bin_table::FilterImage1C;
using ColorTable = bin_table::ColorTable4C;
using TableImage = bin_table::TableImage1C;


/* internal */

namespace util
{
namespace internal
{
    template <typename IMG>
    static bool create_image_gray(IMG& image, u32 width, u32 height)
    {
        img::ImageGray gray;
        if (!img::create_image(gray, width, height))
        {
            return false;
        }

        image.width = width;
        image.height = height;
        image.data = gray.data_;

        return true;
    }
    
    
    template <typename IMG>
    static void destroy_image_gray(IMG& image)
    {
        img::ImageGray gray;
        gray.data_ = image.data;

        img::destroy_image(gray);
        image.data = 0;
    }


    template <typename IMG>
    inline bool write_image_gray(IMG const& image, sfs::path const& path)
    {
        img::ImageGray gray;
        gray.data_ = image.data;
        gray.width = image.width;
        gray.height = image.height;

        return img::write_image(gray, path.c_str());
    }


    template <typename IMG>
    inline bool create_image_rgba(IMG& image, u32 width, u32 height)
    {
        img::Image rgba;
        if (!img::create_image(rgba, width, height))
        {
            return false;
        }

        image.width = rgba.width;
        image.height = rgba.height;
        image.data = rgba.data_;

        return true;
    }


    template <typename IMG>
    inline void destroy_image_rgba(IMG& image)
    {
        img::Image rgba;
        rgba.data_ = image.data;
        img::destroy_image(rgba);
        image.data = 0;
    }


    template <typename IMG>
    inline bool write_image_rgba(IMG const& image, sfs::path const& path)
    {
        img::Image rgba;
        rgba.data_ = image.data;
        rgba.width = image.width;
        rgba.height = image.height;

        return img::write_image(rgba, path.c_str());
    }
}
}


/* mask image */

namespace util
{

    inline bool create_image(MaskImage& image, u32 width, u32 height)
    {
        return internal::create_image_gray(image, width, height);
    }


    inline void destroy_image(MaskImage& image)
    {
        internal::destroy_image_gray(image);
    }


    inline bool write_image(MaskImage const& image, sfs::path const& path)
    {
        return internal::write_image_gray(image, path);
    }    


    static void transform_mask(img::Image const& src, MaskImage const& dst)
    {
        assert(src.data_);
        assert(dst.data);
        assert(src.width == dst.width);
        assert(src.height == dst.height);

        auto length = src.width * src.height;
        auto s = src.data_;
        auto d = dst.data;

        auto on = (u8)FilterKey::Primary;
        auto off = (u8)FilterKey::Transparent;

        for (u32 i = 0; i < length; i++)
        {
            d[i] = s[i].alpha ? on : off;
        }
    }
    
}


/* filter image */

namespace util
{
    static bool create_image(FilterImage& image, u32 width, u32 height)
    {
        return internal::create_image_gray(image, width, height);
    }


    static void destroy_image(FilterImage& image)
    {
        internal::destroy_image_gray(image);
    }


    inline bool write_image(FilterImage const& image, sfs::path const& path)
    {
        return internal::write_image_gray(image, path);
    }


    static void transform_filter(img::Image const& src, FilterImage const& dst, p32 primary, p32 secondary)
    {
        assert(src.data_);
        assert(dst.data);
        assert(src.width == dst.width);
        assert(src.height == dst.height);

        auto length = src.width * src.height;
        auto s = src.data_;
        auto d = dst.data;

        auto equals = [](p32 a, p32 b)
        {
            return a.red == b.red && a.green == b.green && a.blue == b.blue;
        };

        p32 ps;

        for (u32 i = 0; i < length; i++)
        {
            ps = s[i];

            if (!ps.alpha)
            {
                d[i] = (u8)FilterKey::Transparent;                
            }
            else if (equals(ps, primary))
            {
                d[i] = (u8)FilterKey::Primary;
            }
            else if (equals(ps, secondary))
            {
                d[i] = (u8)FilterKey::Secondary;
            }
            else
            {
                d[i] = (u8)FilterKey::Blend;
            }
        }
    }
}


/* table image */

namespace util
{
    static bool create_image(TableImage& image, u32 width, u32 height)
    {
        return internal::create_image_gray(image, width, height);
    }


    static void destroy_image(TableImage& image)
    {
        internal::destroy_image_gray(image);
    }


    inline bool write_image(TableImage const& image, sfs::path const& path)
    {
        return internal::write_image_gray(image, path);
    }
}


/* color table */

namespace util
{
    static bool create_color_table(ColorTable& table)
    {
        img::Image rgba;
        if (!img::create_image(rgba, 256, 1))
        {
            return false;
        }

        constexpr auto none = img::to_pixel(0, 0, 0, 0);

        img::fill(img::make_view(rgba), none);

        table.length = rgba.width;
        table.data = rgba.data_;

        return true;
    }


    static void destroy_color_table(ColorTable& table)
    {
        img::Image rgba;
        rgba.data_ = table.data;
        img::destroy_image(rgba);
        table.data = 0;
    }


    static bool write_color_table(ColorTable& table, sfs::path const& path)
    {
        img::Image rgba;
        rgba.data_ = table.data;
        rgba.width = table.length;
        rgba.height = 1;

        return img::write_image(rgba, path.c_str());
    }


    
}


namespace util
{
    constexpr f32 COEFF_RED = 0.2126f; // 0.299f;
    constexpr f32 COEFF_GREEN = 0.7152f; // 0.587f;
    constexpr f32 COEFF_BLUE = 0.0722f; // 0.114f;


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
    

    inline ColorTable generate_color_table(img::Image const& src)
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

        ColorTable table;
        if (!create_color_table(table))
        {
            assert("*** Image error - color table ***" && false);
            return table;
        }

        for (u32 i = 0; i < N; i++)
        {
            auto& color = colors[i];
            table.data[i] = img::to_pixel(color.red, color.green, color.blue, color.alpha);
        }

        return table;
    }


    inline ColorTable generate_color_table(ImageList<p32> const& list)
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

        ColorTable table;
        if (!create_color_table(table))
        {
            assert("*** Image error - color table ***" && false);
            return table;
        }

        for (u32 i = 0; i < N; i++)
        {
            auto& color = colors[i];
            table.data[i] = img::to_pixel(color.red, color.green, color.blue, color.alpha);
        }

        return table;
    }


    inline TableImage convert_image(img::Image const& src, ColorTable const& table)
    {
        TableImage dst;

        if (!create_image(dst, src.width, src.height))
        {
            assert("*** Image error - convert image ***" && false);
            return dst;
        }

        auto equals = [](p32 a, p32 b)
        {
            return a.red == b.red && a.green == b.green && a.blue == b.blue && a.alpha == b.alpha;
        };

        auto s = img::to_span(src);
        auto d = dst.data;
        auto t = table.data;

        for (u32 i = 0; i < s.length; i++)
        {
            auto ps = s.data[i];
            d[i] = 0;
            
            for (u32 c = 0; c < table.length; c++)
            {
                auto pt = t[c];
                if (equals(pt, ps))
                {
                    d[i] = (u8)c;
                    break;
                }
            }
        }

        return dst;
    }
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
        
        for (auto const& entry : sfs::directory_iterator(dir))
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
        
    
    inline std::string get_variable_name(cstr path, cstr prefix)
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


    inline bool write_image(img::ImageView const& view, sfs::path const& path)
    {
        return img::write_image(img::as_image(view), path.c_str());
    }
}


namespace util
{
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


    static u32 count_write_convert_image_files(PathList const& files, ImageList<p32>& list, ColorTable const& table, sfs::path const& dst_dir)
    {  
        assert(files.size() == list.size());

        u32 count = 0;

        for (u32 i = 0; i < files.size(); i++)
        {
            auto& file = files[i];
            auto& src = list[i];

            auto dst = util::convert_image(src, table);

            auto path = dst_dir / file.filename();
            write_image(dst, path);

            img::destroy_image(src);
            destroy_image(dst);
            count++;
        }

        return count;
    }
}


namespace util
{

}