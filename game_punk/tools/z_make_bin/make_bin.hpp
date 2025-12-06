#pragma once

#include "../utils/tools_include.hpp"
#include "../background/background.hpp"
#include "../sprite/sprite.hpp"
#include "../sky/sky.hpp"
#include "../tile/tile.hpp"
#include "../ui/ui.hpp"

#include "bin_table_types.hpp"
#include "bin_def.hpp"
#include "bin_table_str.hpp"


namespace bin
{
    constexpr u32 KILO = 1024;
    constexpr u32 MEGA = 1024 * KILO;
    constexpr u32 GIGA = 1024 * MEGA;    
    

    // old
    u32 load_directory(u32 offset, cstr dir, InfoList_Image& list, std::ofstream& bin_file, cstr prefix)
    {
        list.offset = offset;

        u32 item_offset = offset;

        auto& items = list.items;

        assert(items.empty() && "*** InfoList_Image must be empty ***");

        if (!items.empty())
        {
            return 0;
        }

        img::Image image;
        for (auto const& entry : sfs::directory_iterator(dir))
        {
            auto& path = entry.path();

            if (!sfs::is_regular_file(path) || path.extension() != ".png")
            {
                continue;
            }

            if (!img::read_image_from_file(path.string().c_str(), image))
            {
                continue;
            }

            FileInfo_Image item;
            item.path = path;
            item.name = util::get_varialbe_name(path.string().c_str(), prefix);
            item.size = 0;
            item.offset = 0;

            item.width = image.width;
            item.height = image.height;

            img::destroy_image(image);

            items.push_back(item);
        }        

        // sort by name
        std::sort(items.begin(), items.end(), [](auto const& a, auto const& b) { return a.name < b.name; });
        
        for (auto& item : items)
        {
            auto buffer = fs::read_bytes(item.path.string().c_str());
            if (!buffer.ok)
            {                
                continue;
            }

            item.size = buffer.size_;
            item.offset = item_offset;

            item_offset += item.size;
            list.size += item.size;

            util::write_buffer(buffer, bin_file);
            mb::destroy_buffer(buffer);
        }

        return item_offset - list.offset; // list.size
    }


    u32 load_image_file(u32 offset, sfs::path const& path, FileInfo_Image& info, std::ofstream& bin_file)
    {
        img::Image image;

        if (!img::read_image_from_file(path.string().c_str(), image))
        {
            return 0;
        }

        auto buffer = fs::read_bytes(path.string().c_str());
        if (!buffer.ok)
        {
            return 0;
        }

        info.path = path;
        info.name = path.stem();
        info.width = image.width;
        info.height = image.height;
        info.offset = offset;
        info.size = buffer.capacity_;

        util::write_buffer(buffer, bin_file);
        mb::destroy_buffer(buffer);
        img::destroy_image(image);
        
        return info.size;        
    }
    
    
    u32 load_directory(u32 offset, sfs::path const& dir, InfoList_Image& list, std::ofstream& bin_file)
    {
        list.offset = offset;

        u32 item_offset = offset;

        auto& items = list.items;

        assert(items.empty() && "*** InfoList_Image must be empty ***");

        if (!items.empty())
        {
            return 0;
        }

        img::Image image;
        for (auto const& entry : sfs::directory_iterator(dir))
        {
            auto& path = entry.path();

            if (!sfs::is_regular_file(path) || path.extension() != ".png")
            {
                continue;
            }

            if (!img::read_image_from_file(path.string().c_str(), image))
            {
                continue;
            }

            FileInfo_Image item;
            item.path = path;
            item.name = path.stem();
            item.size = 0;
            item.offset = 0;

            item.width = image.width;
            item.height = image.height;

            img::destroy_image(image);

            items.push_back(item);
        }        

        // sort by name
        std::sort(items.begin(), items.end(), [](auto const& a, auto const& b) { return a.name < b.name; });
        
        for (auto& item : items)
        {
            auto buffer = fs::read_bytes(item.path.string().c_str());
            if (!buffer.ok)
            {                
                continue;
            }

            item.size = buffer.size_;
            item.offset = item_offset;

            item_offset += item.size;
            list.size += item.size;

            util::write_buffer(buffer, bin_file);
            mb::destroy_buffer(buffer);
        }

        return item_offset - list.offset; // list.size
    }


    u32 load_sky_info(u32 offset, SkyInfo& info, std::ofstream& bin_file)
    {
        constexpr auto base_dir = sky::OUT_SKY_BASE_DIR;
        constexpr auto ov_dir = sky::OUT_SKY_OVERLAY_DIR;
        constexpr auto table_dir = sky::OUT_SKY_TABLE_DIR;

        u32 size = 0;

        info.offset = offset;
        info.size = 0;

        auto& base = info.sky_base;
        auto& overlay = info.sky_overlay;
        auto& table = info.sky_table;
        
        size = load_directory(offset, base_dir, base, bin_file, "base");
        info.size += size;
        offset += size;

        size = load_directory(offset, ov_dir, overlay, bin_file, "ov");
        info.size += size;
        offset += size;

        size = load_directory(offset, table_dir, table, bin_file, "ct");
        info.size += size;
        offset += size;

        return info.size;
    }
    
    
    u32 load_background_info(u32 offset, std::vector<BackgroundInfo>& list, std::ofstream& bin_file)
    {
        u32 begin = offset;

        u32 size = 0;

        auto out_dir = sfs::path(bg::OUT_DIR);

        auto path_table = out_dir / "colors" / "table.png";

        for (auto const& dir : util::get_sub_directories(out_dir))
        {
            if (dir.filename() == "colors") // Magic!
            {
                continue;
            }

            BackgroundInfo info;
            info.offset = offset;
            info.size = 0;
            info.name = dir.filename();

            size = load_directory(offset, dir, info.list, bin_file);
            info.size += size;
            offset += size;

            size = load_image_file(offset, path_table, info.table, bin_file);
            offset += size;

            list.push_back(info);
        }

        return offset - begin;
    }


    u32 load_sprite_info(u32 offset, std::vector<SpritesheetInfo>& list, std::ofstream& bin_file)
    {
        u32 begin = offset;

        for (auto const& dir : util::get_sub_directories(sprite::OUT_DIR))
        {
            SpritesheetInfo info;
            info.offset = offset;
            info.size = 0;
            info.name = dir.filename();

            // Magic!
            auto dir_files = dir / "sprites";
            auto path_table = dir / "table.png";

            auto size = load_directory(offset, dir_files, info.list, bin_file);
            info.size += size;
            offset += size;

            size = load_image_file(offset, path_table, info.table, bin_file);
            offset += size;

            list.push_back(info);
        }

        return offset - begin;
    }


    u32 load_tile_info(u32 offset, std::vector<TileInfo>& list, std::ofstream& bin_file)
    {
        u32 begin = offset;

        for (auto const& dir : util::get_sub_directories(tile::OUT_DIR))
        {
            TileInfo info;
            info.offset = offset;
            info.size = 0;
            info.name = dir.filename();

            // Magic!
            auto dir_files = dir / "tiles";
            auto path_table = dir / "table.png";

            auto size = load_directory(offset, dir_files, info.list, bin_file);
            info.size += size;
            offset += size;

            size = load_image_file(offset, path_table, info.table, bin_file);
            info.size += size;
            offset += size;

            list.push_back(info);
        }

        return offset - begin;
    }


    u32 load_ui_info(u32 offset, std::vector<UIInfo>& list, std::ofstream& bin_file)
    {
        u32 begin = offset;
        

        for (auto const& dir : util::get_sub_directories(ui::OUT_DIR))
        {
            UIInfo info;
            
            info.offset = offset;
            info.size = 0;
            info.name = dir.filename();

            // Magic!
            auto dir_files = dir / "images";
            auto path_table = dir / "table.png";

            auto size = load_directory(offset, dir_files, info.list, bin_file);
            info.size += size;
            offset += size;

            size = load_image_file(offset, path_table, info.table, bin_file);
            info.size += size;
            offset += size;

            list.push_back(info);
        }

        return offset - begin;
    }


    void load_bin_table(BinTableInfo& table, std::ofstream& bin_file)
    {
        u32 offset = 0; // first to load
        u32 size = 0;

        table.size = 0;

        size = load_sky_info(offset, table.sky, bin_file);
        table.size += size;
        offset += size;

        size = load_background_info(offset, table.backgrounds, bin_file);
        table.size += size;
        offset += size;

        size = load_sprite_info(offset, table.spritesheets, bin_file);
        table.size += size;
        offset += size;

        size = load_tile_info(offset, table.tilesets, bin_file);
        table.size += size;
        offset += size;

        size = load_ui_info(offset, table.ui_sets, bin_file);
        table.size += size;
        offset += size;
    }


    void write_bin_table(BinTableInfo const& table)
    {
        using FT = FileType;

        auto out_file_path = sfs::path(OUT_BIN_DIR) / OUT_BIN_TABLE_FILE;
        
        std::ofstream out_file(out_file_path, std::ios::trunc);

        out_file << file_top();
        out_file << define_types();
        out_file << define_info_list_image(table.sky.sky_base, FT::Image4C, "Sky_Base");
        out_file << define_info_list_image(table.sky.sky_overlay, FT::Image1C, "Sky_Overlay");
        out_file << define_info_list_image(table.sky.sky_table, FT::Image4C, "Sky_ColorTable");

        for (auto const& info : table.backgrounds)
        {
            out_file << define_background_set(info);
        }

        for (auto const& info : table.spritesheets)
        {
            out_file << define_sprite_set(info);
        }

        for (auto const& info : table.tilesets)
        {
            out_file << define_tile_set(info);
        }

        for (auto const& info : table.ui_sets)
        {
            out_file << define_ui_set(info);
        }

        

        out_file.close();
    }


    static void print_results(sfs::path const& out_file)
    {
        auto size = fs::file_size(out_file.string().c_str());
        auto name = out_file.filename().string().c_str();;

        auto fsize = (f32)size;
        cstr units = "bytes";

        if (size >= GIGA)
        {
            fsize /= GIGA;
            units = "GB";
        }
        else if (size >= MEGA)
        {
            fsize /= MEGA;
            units = "MB";
        }
        else if (size >= KILO)
        {
            fsize /= KILO;
            units = "KB";
        }

        printf("%s: %4.2f %s\n", name, fsize, units);
        printf("%s\n\n", OUT_BIN_TABLE_FILE);        
    }
    
    
    static void run_make_bin()
    {
        sky::generate_sky();
        bg::generate_backgrounds();
        sprite::generate_sprites();
        tile::generate_tiles();
        ui::generate_ui();
        
        
        sfs::create_directories(OUT_BIN_DIR);

        auto bin_file_path = sfs::path(OUT_BIN_DIR) / OUT_BIN_FILE;

        std::ofstream bin_file(bin_file_path, std::ios::trunc);

        BinTableInfo bin_info;
        load_bin_table(bin_info, bin_file);    

        bin_file.close();

        write_bin_table(bin_info);

        printf("--- data file ---\n");
        print_results(bin_file_path);
    }

}