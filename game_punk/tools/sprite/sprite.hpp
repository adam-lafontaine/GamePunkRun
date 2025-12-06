#pragma once

#include "../utils/tools_include.hpp"


/* spritesheet images */

namespace sprite
{
    class SpritesheetImageResult
    {
    public:
        std::string name;
        PathList files;
        ImageList<p32> list;

        u32 n_expected = 0;
        u32 n_read = 0;
    };


    static SpritesheetImageResult get_spritesheet_images(sfs::path const& dir)
    {
        SpritesheetImageResult result{};
        result.name = dir.filename();
        result.n_expected = 0;
        result.n_read = 0;

        result.files = util::get_png_files(dir);
        result.n_expected += result.files.size();

        result.n_read += util::count_read_image_files_rotate_90(result.files, result.list);

        assert(result.files.size() == result.list.size());

        return result;
    }
}


namespace sprite
{
    static u32 count_write_convert_image_files(PathList const& files, ImageList<p32>& list, img::Image const& table, sfs::path const& dst_dir)
    {  
        assert(files.size() == list.size());

        u32 count = 0;

        for (u32 i = 0; i < files.size(); i++)
        {
            auto& file = files[i];
            auto& src = list[i];

            auto gray = util::convert_image(src, table);

            auto path = dst_dir / file.filename();
            img::write_image(gray, path.c_str());

            img::destroy_image(src);
            img::destroy_image(gray);
            count++;
        }

        return count;
    }

    
    static void print_result(auto const& result, u32 n_written)
    {
        printf("%s: %u/%u/%u\n", result.name.c_str(), result.n_expected, result.n_read, n_written);
    }
}



namespace sprite
{
    inline void generate_sprites()
    {
        auto out_dir = sfs::path(OUT_DIR);

        sfs::create_directories(out_dir);

        printf("\n--- sprites ---\n");

        for (auto const& dir : util::get_sub_directories(SRC_CHARACTER_DIR))
        {
            auto out = out_dir / dir.filename();
            auto out_files = out / "sprites";
            auto out_table = out / "table.png";

            sfs::create_directories(out);
            sfs::create_directories(out_files);            
            
            auto res = get_spritesheet_images(dir);

            auto table = util::create_color_table_image(res.list);
            img::write_image(table, out_table.c_str());            

            u32 n_tile = 0;
            n_tile += count_write_convert_image_files(res.files, res.list, table, out_files);
            
            print_result(res, n_tile);
            img::destroy_image(table);
        }
    }
}

