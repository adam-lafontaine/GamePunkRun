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


    class SkyInfo
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        InfoList_Image sky_base;
        InfoList_Image sky_overlay;
        InfoList_Image sky_table;

    };


    class BackgroundInfo
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        std::string name;

        InfoList_Image list;
        FileInfo_Image table;
    };


    class SpritesheetInfo
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        std::string name;

        InfoList_Image list;
        FileInfo_Image table;
    };


    class TileInfo
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        std::string name;

        InfoList_Image list;
        FileInfo_Image table;
    };    


    class UIInfo
    {
    public:
        u32 offset = 0;
        u32 size = 0;

        std::string name;

        InfoList_Image list;
        FileInfo_Image table;
    };


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