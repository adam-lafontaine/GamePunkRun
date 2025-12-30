#pragma once

namespace bin
{
    using FileType = bin_table::FileType;
    
    
    class FileInfo_Image
    {
    public:
        FileType type = FileType::Unknown;
        u32 width = 0;
        u32 height = 0;

        std::string name;        
        u32 size = 0;
        u32 offset = 0;

        sfs::path path;
    };


    class InfoList_Image
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        std::vector<FileInfo_Image> items;
    };


    class Info_ImageX
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        std::string name;

        InfoList_Image list;
    };


    class Info_ImageX_Table1
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        std::string name;

        InfoList_Image list;
        FileInfo_Image table;
    };


    class Info_ImageX_TableX
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        std::string name;

        InfoList_Image list;
        InfoList_Image tables;
    };


    class SkyBaseInfo : public Info_ImageX {};

    class SkyOverlayInfo : public Info_ImageX_TableX {};


    class SkyInfo
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        SkyBaseInfo sky_base;
        SkyOverlayInfo sky_overlay;
    };


    class BackgroundInfo : public Info_ImageX_Table1 {};

    class SpritesheetInfo : public Info_ImageX_Table1 {};

    class TileInfo : public Info_ImageX_Table1 {};

    class UIInfo : public Info_ImageX_Table1 {};


    class BinTableInfo
    {
    public:

        u32 size = 0;

        SkyInfo sky;
        std::vector<BackgroundInfo> backgrounds;
        std::vector<SpritesheetInfo> spritesheets;
        std::vector<TileInfo> tilesets;
        std::vector<UIInfo> ui_sets;
    };
}