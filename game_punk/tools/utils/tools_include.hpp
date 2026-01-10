#pragma once

#include "../../../libs/io/filesystem.hpp"
#include "../../../libs/image/image.hpp"
#include "../../../libs/datetime/datetime.hpp"
#include "bin_table_types.hpp"

#include <filesystem>
#include <vector>
#include <array>
#include <set>
#include <map>
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
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/image_bin/sky";

    constexpr auto OUT_SKY_BASE_DIR    = "/home/adam/Repos/GamePunkRun/game_punk/tools/make_bin/sky/out_files/base";
    constexpr auto OUT_SKY_OVERLAY_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/make_bin/sky/out_files/overlay";
    constexpr auto OUT_SKY_TABLE_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/make_bin/sky/out_files/table";
}


namespace bg
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/image_bin/backgrounds";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/make_bin/background/out_files/gen";
}


namespace sprite
{
    constexpr auto SRC_CHARACTER_DIR = "/home/adam/Desktop/Game Assets/image_bin/characters";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/make_bin/sprite/out_files/gen";
}


namespace tile
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/image_bin/tiles";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/make_bin/tile/out_files/gen";
}


namespace ui
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/image_bin/ui";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/tools/make_bin/ui/out_files/gen";
}


namespace icon
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/game_icon";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/res/icon";
}


namespace title
{
    constexpr auto SRC_DIR = "/home/adam/Desktop/Game Assets/game_title";

    constexpr auto OUT_DIR = "/home/adam/Repos/GamePunkRun/game_punk/res/title";
}


namespace bin
{
    constexpr auto OUT_BIN_DIR = "/home/adam/Repos/GamePunkRun/game_punk/res/xbin";
    constexpr auto OUT_BIN_FILE = "punk_run.bin";
    constexpr auto OUT_BIN_TABLE_FILE = "bin_table.hpp";

    constexpr auto BIN_TABLE_TYPES_PATH = "/home/adam/Repos/GamePunkRun/game_punk/tools/utils/bin_table_types.hpp";
    
}


using AlphaFilter = bin_table::AlphaFilter;
using AlphaFilterImage = bin_table::AlphaFilterImage;
using ColorTableImage = bin_table::ColorTableImage;
using TableFilterImage = bin_table::TableFilterImage;


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

        image.gray = gray;

        return true;
    }


    template <typename IMG>
    inline bool write_image_gray(IMG const& image, sfs::path const& path)
    {
        return img::write_image(image.gray, path.c_str());
    }


    template <typename IMG>
    inline bool create_image_rgba(IMG& image, u32 width, u32 height)
    {
        img::Image rgba;
        if (!img::create_image(rgba, width, height))
        {
            return false;
        }

        image.rgba = rgba;

        return true;
    }


    template <typename IMG>
    inline bool write_image_rgba(IMG const& image, sfs::path const& path)
    {
        return img::write_image(image.rgba, path.c_str());
    }
}
}


/* filter image */

namespace util
{
    static bool create_image(AlphaFilterImage& image, u32 width, u32 height)
    {
        return internal::create_image_gray(image, width, height);
    }


    inline bool write_image(AlphaFilterImage const& image, sfs::path const& path)
    {
        return internal::write_image_gray(image, path);
    }


    static void transform_filter(img::Image const& src, AlphaFilterImage const& dst, p32 primary, p32 secondary)
    {
        assert(src.data_);
        assert(dst.gray.data_);
        assert(src.width == dst.gray.width);
        assert(src.height == dst.gray.height);

        auto length = src.width * src.height;
        auto s = src.data_;
        auto d = dst.gray.data_;

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
                d[i] = (u8)AlphaFilter::Transparent;                
            }
            else if (equals(ps, primary))
            {
                d[i] = (u8)AlphaFilter::Primary;
            }
            else if (equals(ps, secondary))
            {
                d[i] = (u8)AlphaFilter::Secondary;
            }
            else
            {
                d[i] = (u8)AlphaFilter::Blend;
            }
        }
    }


    static void transform_mask(img::Image const& src, AlphaFilterImage const& dst)
    {
        assert(src.data_);
        assert(dst.gray.data_);
        assert(src.width == dst.gray.width);
        assert(src.height == dst.gray.height);

        auto length = src.width * src.height;
        auto s = src.data_;
        auto d = dst.gray.data_;

        auto on = (u8)AlphaFilter::Primary;
        auto off = (u8)AlphaFilter::Transparent;

        for (u32 i = 0; i < length; i++)
        {
            d[i] = s[i].alpha ? on : off;
        }
    }
}


/* table image */

namespace util
{
    static bool create_image(TableFilterImage& image, u32 width, u32 height)
    {
        return internal::create_image_gray(image, width, height);
    }


    inline bool write_image(TableFilterImage const& image, sfs::path const& path)
    {
        return internal::write_image_gray(image, path);
    }
}


/* color table */

namespace util
{
    static bool create_color_table(ColorTableImage& table)
    {
        img::Image rgba;
        if (!img::create_image(rgba, 256, 1))
        {
            return false;
        }

        constexpr auto none = img::to_pixel(0, 0, 0, 0);

        img::fill(img::make_view(rgba), none);

        table.rgba = rgba;

        return true;
    }


    static bool write_color_table(ColorTableImage& table, sfs::path const& path)
    {
        return img::write_image(table.rgba, path.c_str());
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

    private:
        void set_channels(p32 p)
        {
            if (p.alpha)
            {
                red = p.red;
                green = p.green;
                blue = p.blue;
                alpha = p.alpha;

                gray = rgb_to_gray(red, green, blue);
            }
            else
            {
                gray = -1.0f; // transparent first element in table
            }
        }

    public:

        TableColor() = delete;

        TableColor(p32 p)
        {
            set_channels(p);
        }

        TableColor(u32 n)
        {
            union 
            {
                u32 n;
                p32 p;
            } conv;

            conv.n = n;

            set_channels(conv.p);
        }

        bool operator < (TableColor const& other) { return gray < other.gray; }
    };
    

    inline ColorTableImage generate_color_table(img::Image const& src)
    {
        std::set<u32> unique;

        auto const insert = [&](p32 p)
        {
            union 
            {
                u32 n;
                p32 p;
            } conv;

            conv.p = p;

            unique.insert(conv.n);
        };
        
        img::for_each_pixel(img::make_view(src), insert);

        std::vector<TableColor> colors(unique.begin(), unique.end());
        std::sort(colors.begin(), colors.end());

        u32 N = colors.size();

        assert(N <= 256);

        ColorTableImage table;
        if (!create_color_table(table))
        {
            assert("*** Image error - color table ***" && false);
            return table;
        }

        for (u32 i = 0; i < N; i++)
        {
            auto& color = colors[i];
            table.rgba.data_[i] = img::to_pixel(color.red, color.green, color.blue, color.alpha);
        }

        return table;
    }


    inline ColorTableImage generate_color_table(ImageList<p32> const& list)
    {
        std::set<u32> unique;

        auto const insert = [&](p32 p)
        {
            union 
            {
                u32 n;
                p32 p;
            } conv;

            conv.p = p;

            unique.insert(conv.n);
        };
        
        for (auto const& item : list)
        {
            img::for_each_pixel(img::make_view(item), insert);
        }

        std::vector<TableColor> colors(unique.begin(), unique.end());
        std::sort(colors.begin(), colors.end());

        u32 N = colors.size();

        assert(N <= 256);

        ColorTableImage table;
        if (!create_color_table(table))
        {
            assert("*** Image error - color table ***" && false);
            return table;
        }

        for (u32 i = 0; i < N; i++)
        {
            auto& color = colors[i];
            table.rgba.data_[i] = img::to_pixel(color.red, color.green, color.blue, color.alpha);
        }

        return table;
    }


    inline TableFilterImage convert_image(img::Image const& src, ColorTableImage const& table)
    {
        TableFilterImage dst;

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
        auto d = dst.gray.data_;
        auto t = img::to_span(table.rgba);

        for (u32 i = 0; i < s.length; i++)
        {
            auto ps = s.data[i];
            d[i] = 0;
            
            for (u32 c = 0; c < t.length; c++)
            {
                auto pt = t.data[c];
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
    inline u32 count_read_image_rotate_90(sfs::path const& path, img::Image& dst)
    {
        img::Image png;
        if (!img::read_image_from_file(path.c_str(), png))
        {
            return 0;
        }

        if (!img::create_image(dst, png.height, png.width))
        {
            img::destroy_image(png);
            return 0;
        }

        img::rotate_90(img::make_view(png), img::make_view(dst));

        img::destroy_image(png);

        return 1;
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


    static u32 count_write_convert_image_files(PathList const& files, ImageList<p32>& list, ColorTableImage const& table, sfs::path const& dst_dir)
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
            dst.destroy();
            count++;
        }

        return count;
    }
}


namespace util
{
    static std::string image_cpp_string(img::Image src, cstr name)
    {
        auto data = (u32*)src.data_;
        auto length = src.width * src.height;

        std::set<u32> table_values;
        std::map<u32, i32> table_value_keys;        
        std::vector<u32> table_key_values;

        for (u32 i = 0; i < length; i++)
        {
            table_values.insert(data[i]);
        }

        auto kv_len = table_values.size();
        assert(kv_len <= 256);

        i32 index = 0;
        for (auto value : table_values)
        {
            table_value_keys[value] = index++;
            table_key_values.push_back(value);
        }

        u32 max_table_w = 8;
        u32 table_w = kv_len < max_table_w ? kv_len : max_table_w;
        u32 table_h = kv_len / table_w + 1;


        std::ostringstream oss;

        oss << "    const struct\n";
        oss << "    {\n";

        oss << "        unsigned int  width;\n";
        oss << "        unsigned int  height;\n";
        oss << "        unsigned int table[" << kv_len << "];\n";
        oss << "        unsigned char keys[" << length << "];\n";        

        oss << "    } " << name << "\n";
        oss << "    {\n";

        oss << "        " << src.width << ", // width\n";
        oss << "        " << src.height << ", // height\n\n";
        
        oss << "        { // table\n";

        u32 i = 0;
        for (u32 y = 0; y < table_h; y++)
        {
            oss << "            ";
            for (u32 x = 0; x < table_w && i < kv_len; x++)
            {
                oss << "0x" << std::hex << table_key_values[i++] << ", ";
            }
            oss << "\n";
        }

        oss << "        },\n\n";
        
        oss << "        { // keys\n";
        
        i = 0;
        for (u32 y = 0; y < src.height; y++)
        {
            oss << "            ";
            for (u32 x = 0; x < src.width; x++)
            {
                oss << table_value_keys[data[i++]] << ", ";
            }
            oss << "\n";
        }

        oss << "        }\n";

        oss << "    };\n";

        return oss.str();
    }
}