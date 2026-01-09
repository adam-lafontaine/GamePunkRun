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

        case FT::Image1C: return "FileType::Image1C";
        
        case FT::Image1C_AlphaFilter: return "FileType::Image1C_AlphaFilter";
        case FT::Image1C_TableFilter:  return "FileType::Image1C_TableFilter";

        case FT::Music: return "FileType::Music";
        case FT::SFX:   return "FileType::SFX";

        default: return "FileType::Unknown";
        }
    }


    std::string define_image_set(bin::Info_ImageX const& info, cstr set_class, bin::FileType image_type)
    {
        auto& set_name = info.name;
        auto set_offset = (int)info.offset;
        auto set_size = (int)info.size;
        auto file_type = xbin::to_cstr(image_type);
        auto& items = info.list.items;
        auto item_count = (int)info.list.items.size();

        std::ostringstream oss;
        i32 t = 1;

        auto const file_info_items = [&]()
        {
            xbin::oss_tab(oss, t) << "ImageInfo items[count] = {\n";
            t++;
            for (auto const& item : items)
            {
                auto w = item.width;
                auto h = item.height;
                auto name = std::string("\"") +  item.name + '"';
                auto offset = (int)item.offset;
                auto size = (int)item.size;

                    xbin::oss_tab(oss, t) << "to_image_info(file_type, " << w << ", " << h << ", ";
                    oss << name << ", " << offset << ", " << size << "),\n";
            }
            t--;
            xbin::oss_tab(oss, t) << "};\n\n";

            xbin::oss_tab(oss, t) << "enum class Items : u32\n";
            xbin::oss_tab(oss, t) << "{\n";
            t++;
            for (auto const& item : items)
            {
                 xbin::oss_tab(oss, t) << item.name << ",\n";
            }
            t--;
            xbin::oss_tab(oss, t) <<"};\n\n\n";
        };

        auto const constructor = [&]()
        {
            xbin::oss_tab(oss, t) << "constexpr " << set_class << "_" << set_name << "(){}\n\n\n";
        };

        auto const method_test = [&]()
        {
            // bool test(Buffer8 const& buffer)
            xbin::oss_tab(oss, t) << "bool test(Buffer8 const& buffer) const\n";
            xbin::oss_tab(oss, t) << "{\n";
            t++;
                xbin::oss_tab(oss, t) << "return test_items(buffer, (ImageInfo*)items, count);\n";
            t--;
            xbin::oss_tab(oss, t) << "}\n\n\n";
        };

        auto const method_read_item = [&]()
        {
            switch (image_type)
            {
            case bin::FileType::Image1C:
            {               
                xbin::oss_tab(oss, t) << "Image read_gray_item(Buffer8 const& buffer, Items key) const\n";
                xbin::oss_tab(oss, t) << "{\n";
                t++;
                    xbin::oss_tab(oss, t) << "return read_gray(buffer, items[(u32)key]);\n";
                t--;
                xbin::oss_tab(oss, t) << "}\n\n\n";
            } break;

            case bin::FileType::Image4C:
            {
                xbin::oss_tab(oss, t) << "Image read_rgba_item(Buffer8 const& buffer, Items key) const\n";
                xbin::oss_tab(oss, t) << "{\n";
                t++;
                    xbin::oss_tab(oss, t) << "return read_rgba(buffer, items[(u32)key]);\n";
                t--;
                xbin::oss_tab(oss, t) << "}\n\n\n";
            } break;

            default:
            {
                xbin::oss_tab(oss, t) << "#error Image1C or Image4C expected\n";
            } break;
            }
        };        
        
        xbin::ns_begin(oss);       

        xbin::oss_tab(oss, t) << "// define_image_set(Info_ImageX)\n";

        xbin::oss_tab(oss, t) << "class " << set_class << "_" << set_name << "\n";
        xbin::oss_tab(oss, t) << "{\n";
        xbin::oss_tab(oss, t) << "public:\n";
        t++;
            xbin::oss_tab(oss, t) << "using ImageInfo = AssetInfo_Image;\n\n";

            xbin::oss_tab(oss, t) << "static constexpr u32 offset = " << set_offset << ";\n";
            xbin::oss_tab(oss, t) << "static constexpr u32 size = " << set_size << ";\n\n";

            xbin::oss_tab(oss, t) << "static constexpr FileType file_type = " << file_type <<";\n";
            xbin::oss_tab(oss, t) << "static constexpr u32 count = " << item_count <<";\n\n";            

        file_info_items();
        constructor();
        method_test();
        method_read_item();            

        t--;
        xbin::oss_tab(oss, t) << "};\n";

        xbin::ns_end(oss);

        return oss.str();
    }


    static std::string define_image_set(bin::Info_ImageX_Table1 const& info, cstr set_class, bin::FileType image_type)
    {
        auto& set_name = info.name;
        auto set_offset = (int)info.offset;
        auto set_size = (int)info.size;
        auto file_type = xbin::to_cstr(image_type);
        auto table_type = xbin::to_cstr(bin::FileType::Image4C_Table);
        auto& items = info.list.items;
        auto item_count = (int)info.list.items.size();

        std::ostringstream oss;
        i32 t = 1;


        auto const file_info_items = [&]()
        {
            xbin::oss_tab(oss, t) << "ImageInfo items[count] = {\n";
            t++;
            for (auto const& item : items)
            {
                auto w = item.width;
                auto h = item.height;
                auto name = std::string("\"") +  item.name + '"';
                auto offset = (int)item.offset;
                auto size = (int)item.size;

                    xbin::oss_tab(oss, t) << "to_image_info(file_type, " << w << ", " << h << ", ";
                    oss << name << ", " << offset << ", " << size << "),\n";
            }
            t--;
            xbin::oss_tab(oss, t) << "};\n\n";

            xbin::oss_tab(oss, t) << "enum class Items : u32\n";
            xbin::oss_tab(oss, t) << "{\n";
            t++;
            for (auto const& item : items)
            {
                 xbin::oss_tab(oss, t) << item.name << ",\n";
            }
            t--;
            xbin::oss_tab(oss, t) <<"};\n\n\n";
        };

        auto const color_table = [&]()
        {
            auto w = info.table.width;
            auto h = info.table.height;
            auto name = std::string("\"") +  info.table.name + '"';
            auto offset = (int)info.table.offset;
            auto size = (int)info.table.size;

            xbin::oss_tab(oss, t) << "static constexpr ImageInfo color_table = ";
            oss << "to_image_info(table_type, " << w << ", " << h << ", ";
            oss  << name << ", " << offset << ", " << size << ");\n\n";
        };

        auto const constructor = [&]()
        {
            xbin::oss_tab(oss, t) << "constexpr " << set_class << "_" << set_name << "(){}\n\n\n";
        };

        auto const method_test = [&]()
        {
            // bool test(Buffer8 const& buffer)
            xbin::oss_tab(oss, t) << "bool test(Buffer8 const& buffer) const\n";
            xbin::oss_tab(oss, t) << "{\n";
            t++;
                xbin::oss_tab(oss, t) << "return test_items(buffer, (ImageInfo*)items, count) && test_read(buffer, color_table);\n";
            t--;
            xbin::oss_tab(oss, t) << "}\n\n\n";
        };

        auto const method_read_table = [&]()
        {
            // ColorTableImage read_table(Buffer8 const& buffer)
            xbin::oss_tab(oss, t) << "ColorTableImage read_table(Buffer8 const& buffer) const\n";
            xbin::oss_tab(oss, t) << "{\n";
            t++;
                xbin::oss_tab(oss, t) << "static_assert(ColorTableImage::type == table_type);\n";
                xbin::oss_tab(oss, t) << "return read_color_table(buffer, color_table);\n";
            t--;
            xbin::oss_tab(oss, t) << "}\n\n\n";
        };

        auto const method_read_item = [&]()
        {
            switch (image_type)
            {
            case bin::FileType::Image1C_AlphaFilter:
            {
                xbin::oss_tab(oss, t) << "AlphaFilterImage read_alpha_filter_item(Buffer8 const& buffer, Items key) const\n";
                xbin::oss_tab(oss, t) << "{\n";
                t++;
                    xbin::oss_tab(oss, t) << "static_assert(AlphaFilterImage::type == file_type);\n";
                    xbin::oss_tab(oss, t) << "return read_alpha_filter(buffer, items[(u32)key]);\n";
                t--;
                xbin::oss_tab(oss, t) << "}\n\n\n";
            } break;

            case bin::FileType::Image1C_TableFilter:
            {
                xbin::oss_tab(oss, t) << "TableFilterImage read_table_filter_item(Buffer8 const& buffer, Items key) const\n";
                xbin::oss_tab(oss, t) << "{\n";
                t++;
                    xbin::oss_tab(oss, t) << "static_assert(TableFilterImage::type == file_type);\n";
                    xbin::oss_tab(oss, t) << "return read_table_filter(buffer, items[(u32)key]);\n";
                t--;
                xbin::oss_tab(oss, t) << "}\n\n\n";
            } break;

            default:
            {
                xbin::oss_tab(oss, t) << "#error Image1C_AlphaFilter or Image1C_TableFilter expected\n";
            } break;
            }
        };

        xbin::ns_begin(oss);

        xbin::oss_tab(oss, t) << "// define_image_set(Info_ImageX_Table1)\n";

        xbin::oss_tab(oss, t) << "class " << set_class << "_" << set_name << "\n";
        xbin::oss_tab(oss, t) << "{\n";
        xbin::oss_tab(oss, t) << "public:\n";
        t++;
            xbin::oss_tab(oss, t) << "static constexpr u32 offset = " << set_offset << ";\n";
            xbin::oss_tab(oss, t) << "static constexpr u32 size = " << set_size << ";\n\n";

            xbin::oss_tab(oss, t) << "using ImageInfo = AssetInfo_Image;\n\n";

            xbin::oss_tab(oss, t) << "static constexpr FileType file_type = " << file_type <<";\n";
            xbin::oss_tab(oss, t) << "static constexpr FileType table_type = " << table_type <<";\n\n";

            xbin::oss_tab(oss, t) << "static constexpr u32 count = " << item_count <<";\n\n";

        file_info_items();
        color_table();
        constructor();
        method_test();
        method_read_table();
        method_read_item();

        t--;
        xbin::oss_tab(oss, t) << "};\n";

        xbin::ns_end(oss);

        return oss.str();
    }


    static std::string define_image_set(bin::Info_ImageX_TableX const& info, cstr set_class, bin::FileType image_type)
    {
        auto& set_name = info.name;
        auto set_offset = (int)info.offset;
        auto set_size = (int)info.size;
        auto file_type = xbin::to_cstr(image_type);
        auto table_type = xbin::to_cstr(bin::FileType::Image4C_Table);
        auto& items = info.list.items;
        auto& tables = info.tables.items;
        auto item_count = (int)info.list.items.size();

        std::ostringstream oss;
        i32 t = 1;

        auto const file_info_items = [&]()
        {
            xbin::oss_tab(oss, t) << "ImageInfo items[count] = {\n";
            t++;
            for (auto const& item : items)
            {
                auto w = item.width;
                auto h = item.height;
                auto name = std::string("\"") +  item.name + '"';
                auto offset = (int)item.offset;
                auto size = (int)item.size;

                    xbin::oss_tab(oss, t) << "to_image_info(file_type, " << w << ", " << h << ", ";
                    oss << name << ", " << offset << ", " << size << "),\n";
            }
            t--;
            xbin::oss_tab(oss, t) << "};\n\n";

            xbin::oss_tab(oss, t) << "enum class Items : u32\n";
            xbin::oss_tab(oss, t) << "{\n";
            t++;
            for (auto const& item : items)
            {
                 xbin::oss_tab(oss, t) << item.name << ",\n";
            }
            t--;
            xbin::oss_tab(oss, t) <<"};\n\n\n";
        };

        auto const color_table_items = [&]()
        {
            xbin::oss_tab(oss, t) << "ImageInfo table_items[count] = {\n";
            t++;
            for (auto const& item : tables)
            {
                auto w = item.width;
                auto h = item.height;
                auto name = std::string("\"") +  item.name + '"';
                auto offset = (int)item.offset;
                auto size = (int)item.size;

                    xbin::oss_tab(oss, t) << "to_image_info(table_type, " << w << ", " << h << ", ";
                    oss << name << ", " << offset << ", " << size << "),\n";
            }
            t--;
            xbin::oss_tab(oss, t) << "};\n\n\n";
        };

        auto const constructor = [&]()
        {
            xbin::oss_tab(oss, t) << "constexpr " << set_class << "_" << set_name << "(){}\n\n\n";
        };

        auto const method_test = [&]()
        {
            // bool test(Buffer8 const& buffer)
            xbin::oss_tab(oss, t) << "bool test(Buffer8 const& buffer) const\n";
            xbin::oss_tab(oss, t) << "{\n";
            t++;
                xbin::oss_tab(oss, t) << "return test_items(buffer, (ImageInfo*)items, count) && test_items(buffer, (ImageInfo*)table_items, count);\n";
            t--;
            xbin::oss_tab(oss, t) << "}\n\n\n";
        };

        auto const method_read_item = [&]()
        {
            switch (image_type)
            {
            case bin::FileType::Image1C_AlphaFilter:
            {
                xbin::oss_tab(oss, t) << "AlphaFilterImage read_alpha_filter_item(Buffer8 const& buffer, Items key) const\n";
                xbin::oss_tab(oss, t) << "{\n";
                t++;
                    xbin::oss_tab(oss, t) << "static_assert(AlphaFilterImage::type == file_type);\n";
                    xbin::oss_tab(oss, t) << "return read_alpha_filter(buffer, items[(u32)key]);\n";
                t--;
                xbin::oss_tab(oss, t) << "}\n\n\n";
            } break;

            case bin::FileType::Image1C_TableFilter:
            {
                xbin::oss_tab(oss, t) << "TableFilterImage read_table_filter_item(Buffer8 const& buffer, Items key) const\n";
                xbin::oss_tab(oss, t) << "{\n";
                t++;
                    xbin::oss_tab(oss, t) << "static_assert(TableFilterImage::type == file_type);\n";
                    xbin::oss_tab(oss, t) << "return read_table_filter(buffer, items[(u32)key]);\n";
                t--;
                xbin::oss_tab(oss, t) << "}\n\n\n";
            } break;

            default:
            {
                xbin::oss_tab(oss, t) << "#error Image1C_AlphaFilter or Image1C_TableFilter expected\n";
            } break;
            }
        };

        auto const method_read_table = [&]()
        {
            // ColorTableImage read_table(Buffer8 const& buffer)
            xbin::oss_tab(oss, t) << "ColorTableImage read_table_item(Buffer8 const& buffer, Items key) const\n";
            xbin::oss_tab(oss, t) << "{\n";
            t++;
                xbin::oss_tab(oss, t) << "static_assert(ColorTableImage::type == table_type);\n";
                xbin::oss_tab(oss, t) << "return read_color_table(buffer, table_items[(u32)key]);\n";
            t--;
            xbin::oss_tab(oss, t) << "}\n\n\n";
        };

        xbin::ns_begin(oss);

        xbin::oss_tab(oss, t) << "// define_image_set(Info_ImageX_TableX)\n";

        xbin::oss_tab(oss, t) << "class " << set_class << "_" << set_name << "\n";
        xbin::oss_tab(oss, t) << "{\n";
        xbin::oss_tab(oss, t) << "public:\n";
        t++;
            xbin::oss_tab(oss, t) << "static constexpr u32 offset = " << set_offset << ";\n";
            xbin::oss_tab(oss, t) << "static constexpr u32 size = " << set_size << ";\n\n";

            xbin::oss_tab(oss, t) << "using ImageInfo = AssetInfo_Image;\n\n";

            xbin::oss_tab(oss, t) << "static constexpr FileType file_type = " << file_type <<";\n";
            xbin::oss_tab(oss, t) << "static constexpr FileType table_type = " << table_type <<";\n\n";

            xbin::oss_tab(oss, t) << "static constexpr u32 count = " << item_count <<";\n\n";

        file_info_items();
        color_table_items();
        constructor();
        method_test();
        method_read_item();
        method_read_table();       

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


    static u32 class_count = 0;


    std::string define_constants(BinTableInfo const& info)
    {
        std::ostringstream oss;

        xbin::ns_begin(oss);
        xbin::oss_tab(oss, 1) << "constexpr u32 VERSION = " << info.version_number << ";\n";
        xbin::oss_tab(oss, 1) << "constexpr u32 CLASS_COUNT = " << class_count << ";\n";
        xbin::ns_end(oss);

        return oss.str();
    }


    std::string define_sky_base_set(SkyBaseInfo const& info)
    {
        class_count++;

        return xbin::define_image_set(info, "SkyBase", bin::FileType::Image4C);
    }


    std::string define_sky_overlay_set(SkyOverlayInfo const& info)
    {
        class_count++;

        return xbin::define_image_set(info, "SkyOverlay", bin::FileType::Image1C_TableFilter);
    }


    std::string define_background_set(BackgroundInfo const& info)
    {
        class_count++;

        return xbin::define_image_set(info, "Background", bin::FileType::Image1C_AlphaFilter);
    }
    
    
    std::string define_sprite_set(SpritesheetInfo const& info)
    {
        class_count++;

        return xbin::define_image_set(info, "Spriteset", bin::FileType::Image1C_TableFilter);
    }
    
    
    std::string define_tile_set(TileInfo const& info)
    {
        class_count++;

        return xbin::define_image_set(info, "Tileset", bin::FileType::Image1C_TableFilter);
    }


    std::string define_ui_set(UIInfo const& info)
    {
        class_count++;
        
        return xbin::define_image_set(info, "UIset", bin::FileType::Image1C_TableFilter);
    }
}