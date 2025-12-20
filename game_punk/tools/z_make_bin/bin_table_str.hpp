#pragma once


/* helpers */

namespace xbin
{    
    static void ns_begin(std::ostringstream& oss)
    {
        oss << "// auto-generated\n";
        oss << "namespace bin_table\n{\n\n";
    }


    static void ns_end(std::ostringstream& oss)
    {
        oss << "\n}\n\n\n";
    }


    static cstr tab(i32 n = 1)
    {
        switch (n)
        {
        case 1: return "\t";
        case 2: return "\t\t";
        case 3: return "\t\t\t";
        case 4: return "\t\t\t\t";
        case 5: return "\t\t\t\t\t";
        case 6: return "\t\t\t\t\t\t";
        case 7: return "\t\t\t\t\t\t\t";
        case 8: return "\t\t\t\t\t\t\t\t";
        default: break;
        }

        assert("*** Need more tabs ***" && false);
    }


    static std::ostringstream& oss_tab(std::ostringstream& oss, i32 n)
    {
        oss << tab(n);

        return oss;
    }


    static cstr to_cstr(bin::FileType type)
    {
        using FT = bin::FileType;

        switch (type)
        {
        case FT::Image4C: return "FileType::Image4C";

        case FT::Image4C_Table: return "FileType::Image4C_Table";        

        case FT::Image4C_Spritesheet: return "FileType::Image4C_Spritesheet";
        case FT::Image4C_Tile:        return "FileType::Image4C_Tile";

        case FT::Image1C: return "FileType::Image1C";

        case FT::Image1C_Mask:   return "FileType::Image1C_Mask";
        case FT::Image1C_Filter: return "FileType::Image1C_Filter";
        case FT::Image1C_Table:  return "FileType::Image1C_Table";

        case FT::Music: return "FileType::Music";
        case FT::SFX:   return "FileType::SFX";

        default: return "FileType::Unknown";
        }
    }


    static std::string define_image_set(auto const& info, cstr set_class, bin::FileType image_type)
    {
        auto& set_name = info.name;
        auto set_offset = (int)info.offset;
        auto set_size = (int)info.size;
        auto file_type = xbin::to_cstr(image_type);
        auto table_type = xbin::to_cstr(bin::FileType::Image4C_Table);
        auto& items = info.list.items;
        auto item_count = (int)info.list.items.size();

        std::ostringstream oss;

        xbin::ns_begin(oss);

        i32 t = 1;

        xbin::oss_tab(oss, t) << "// define_image_set()\n";

        xbin::oss_tab(oss, t) << "class " << set_class << "_" << set_name << "\n";
        xbin::oss_tab(oss, t) << "{\n";
        xbin::oss_tab(oss, t) << "public:\n";
        t++;
            xbin::oss_tab(oss, t) << "u32 offset = " << set_offset << ";\n";
            xbin::oss_tab(oss, t) << "u32 size = " << set_size << ";\n\n";

            xbin::oss_tab(oss, t) << "static constexpr FileType file_type = " << file_type <<";\n";
            xbin::oss_tab(oss, t) << "static constexpr FileType table_type = " << table_type <<";\n\n";

            xbin::oss_tab(oss, t) << "static constexpr auto uFT = (u8)file_type;\n";
            xbin::oss_tab(oss, t) << "static constexpr auto uTT = (u8)table_type;\n\n";

            xbin::oss_tab(oss, t) << "using ImageInfo = FileInfo_Image<uFT>;\n";
            xbin::oss_tab(oss, t) << "using TableInfo = FileInfo_Image<uTT>;\n\n";

            xbin::oss_tab(oss, t) << "static constexpr u32 count = " << item_count <<";\n\n";

            xbin::oss_tab(oss, t) << "union\n";
            xbin::oss_tab(oss, t) << "{\n";
        t++;
                xbin::oss_tab(oss, t) << "ImageInfo items[count] = {\n";
        t++;
        for (auto const& item : items)
        {
            auto w = item.width;
            auto h = item.height;
            auto name = std::string("\"") +  item.name + '"';
            auto offset = (int)item.offset;
            auto size = (int)item.size;

                    xbin::oss_tab(oss, t) << "to_file_info_image<uFT>(" << w << ", " << h << ", " << name << ", " << offset << ", " << size << "),\n";
        }
        t--;
                xbin::oss_tab(oss, t) << "};\n\n";

                xbin::oss_tab(oss, t) << "struct\n";
                xbin::oss_tab(oss, t) << "{\n";
        t++;
        for (auto const& item : items)
        {
                    xbin::oss_tab(oss, t) << "ImageInfo " << item.name << ";\n";
        }
        t--;

                xbin::oss_tab(oss, t) << "} file_info;\n";
        t--;
            xbin::oss_tab(oss, t) << "};\n\n";


        auto w = info.table.width;
        auto h = info.table.height;
        auto name = std::string("\"") +  info.table.name + '"';
        auto offset = (int)info.table.offset;
        auto size = (int)info.table.size;
            xbin::oss_tab(oss, t) << "static constexpr TableInfo color_table = to_file_info_image<uTT>(" << w << ", " << h << ", " << name << ", " << offset << ", " << size << ");\n\n";

            xbin::oss_tab(oss, t) << "constexpr " << set_class << "_" << set_name << "(){}\n";
        t--;
        xbin::oss_tab(oss, t) << "};\n";

        xbin::ns_end(oss);

        return oss.str();
    }
}


/* api */

namespace bin
{
    std::string file_top()
    {
        auto ts = dt::current_timestamp_i64();

        std::ostringstream oss;

        oss << "#pragma once\n";
        oss << "/* timestamp: " << ts << " */\n\n\n";

        return oss.str();
    }


    std::string define_types()
    {
        std::ifstream file(BIN_TABLE_TYPES_PATH);
        if (!file.is_open())
        {
            return "/* Error: bin_table_types.hpp */";
        }

        std::ostringstream oss;

        std::string line;
        while (std::getline(file, line))
        {
            oss << line << "\n";
        }

        oss << "\n\n";

        file.close();

        return oss.str();
    }


    std::string define_info_list_image(InfoList_Image const& list, FileType type, cstr class_tag)
    {
        auto list_offset = (int)list.offset;
        auto list_size = (int)list.size;
        auto file_type = xbin::to_cstr(type);
        auto item_count = (int)list.items.size();

        std::ostringstream oss;

        xbin::ns_begin(oss);

        i32 t = 1;

        xbin::oss_tab(oss, t) << "// define_info_list_image()\n";

        xbin::oss_tab(oss, t) << "class InfoList_Image_" << class_tag << "\n";
        xbin::oss_tab(oss, t) << "{\n";
        xbin::oss_tab(oss, t) << "public:\n";
        t++;
            xbin::oss_tab(oss, t) << "u32 offset = " << list_offset << ";\n";
            xbin::oss_tab(oss, t) << "u32 size = " << list_size << ";\n\n";

            xbin::oss_tab(oss, t) << "static constexpr FileType file_type = " << file_type <<";\n";
            xbin::oss_tab(oss, t) << "static constexpr auto uFT = (u8)file_type;\n";
            xbin::oss_tab(oss, t) << "using ImageInfo = FileInfo_Image<uFT>;\n\n";

            xbin::oss_tab(oss, t) << "static constexpr u32 count = " << item_count <<";\n";            
            

            xbin::oss_tab(oss, t) << "union\n";
            xbin::oss_tab(oss, t) << "{\n";
        t++;
                xbin::oss_tab(oss, t) << "ImageInfo items[count] = {\n";
        t++;
        for (auto const& item : list.items)
        {
            auto w = item.width;
            auto h = item.height;
            auto name = std::string("\"") +  item.name + '"';
            auto offset = (int)item.offset;
            auto size = (int)item.size;

                    xbin::oss_tab(oss, t) << "to_file_info_image<uFT>(" << w << ", " << h << ", " << name << ", " << offset << ", " << size << "),\n";
        }
        t--;
                xbin::oss_tab(oss, t) << "};\n\n";

                xbin::oss_tab(oss, t) << "struct\n";
                xbin::oss_tab(oss, t) << "{\n";
        t++;
        for (auto const& item : list.items)
        {
                    xbin::oss_tab(oss, t) << "ImageInfo " << item.name << ";\n";
        }
        t--;

                xbin::oss_tab(oss, t) << "} file_info;\n";
        t--;
            xbin::oss_tab(oss, t) << "};\n\n";

            xbin::oss_tab(oss, t) << "constexpr InfoList_Image_" << class_tag << "(){}\n";
        t--;
        xbin::oss_tab(oss, t) << "};\n";

        xbin::ns_end(oss);

        return oss.str();
    }


    std::string define_background_set(BackgroundInfo const& info)
    {
        return xbin::define_image_set(info, "Background", bin::FileType::Image1C_Mask);
    }
    
    
    std::string define_sprite_set(SpritesheetInfo const& info)
    {
        return xbin::define_image_set(info, "Spriteset", bin::FileType::Image1C_Table);
    }
    
    
    std::string define_tile_set(TileInfo const& info)
    {
        return xbin::define_image_set(info, "Tileset", bin::FileType::Image1C_Table);
    }


    std::string define_ui_set(UIInfo const& info)
    {
        return xbin::define_image_set(info, "UIset", bin::FileType::Image1C_Filter);
    }
}