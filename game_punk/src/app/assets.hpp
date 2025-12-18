#pragma once

#include "../../../libs/io/filesystem.hpp"


/* definitions */

namespace game_punk
{
namespace assets
{
    using ImageInfo = bt::FileInfo_Image;


    constexpr f32 SKY_OVERLAY_ALPHA = 0.22f;
    constexpr f32 SKY_BASE_ALPHA = 1.0f - SKY_OVERLAY_ALPHA;

}
}


/* helpers */

namespace game_punk
{
namespace assets
{

    static void mask_convert(Span8 const& src, Span32 const& dst)
    {
        for (u32 i = 0; i < src.length; i++)
        {
            dst.data[i] = src.data[i] ? COLOR_WHITE : COLOR_TRANSPARENT;
        }
    }
    
    
    static void mask_convert(ImageGray const& src_mask, ImageView const& dst_view)
    {
        auto src = img::to_span(src_mask);
        auto dst = img::to_span(dst_view);

        mask_convert(src, dst);
    }


    static void filter_convert(Span8 const& src, Span32 const& dst)
    {
        for (u32 i = 0; i < src.length; i++)
        {
            auto p = src.data[i];
            dst.data[i] = img::to_pixel(p, p, p, p);
        }
    }


    static void filter_convert(ImageGray const& src_mask, ImageView const& dst_view)
    {
        auto src = img::to_span(src_mask);
        auto dst = img::to_span(dst_view);

        filter_convert(src, dst);
    }
    
    
    static void extend_view_x(Image const& src_view, ImageView const& dst_view)
    {
        auto src = img::make_view(src_view);

        auto w = src.width;
        auto h = src.height;

        auto rect = img::make_rect(w, h);

        auto N = dst_view.width / w;

        for (u32 i = 0; i < N; i++)
        {
            auto dst = img::sub_view(dst_view, rect);
            img::copy(src, dst);
            rect.x_begin += w;
            rect.x_end += w;
        }
    }


    static void extend_view_y(Image const& src_view, ImageView const& dst_view)
    {
        auto src = img::to_span(src_view);
        auto dst = img::to_span(dst_view);
        dst.length = src.length;

        auto w = src_view.width;
        auto h = src_view.height;

        auto N = dst_view.height / h;

        for (u32 i = 0; i < N; i++)
        {
            span::copy(src, dst);
            dst.data += dst.length;
        }
    }
           
    
    static void apply_pma(Span32 const& src, Span32 const& dst, f32 alpha)
    {
        f32 r = 0.0f;
        f32 g = 0.0f;
        f32 b = 0.0f;

        auto do_pma = [&](p32 ps)
        {
            r = ps.red * alpha + 0.5f;
            g = ps.green * alpha + 0.5f;
            b = ps.blue * alpha + 0.5f;

            return img::to_pixel((u8)r, (u8)g, (u8)b);
        };

        span::transform(src, dst, do_pma);
    } 
    
    
    static void apply_color_table(Span8 const& src, Span32 const& dst, Span32 const& table)
    {
        p32 ps;

        for (u32 i = 0; i < src.length; i++)
        {
            dst.data[i] = table.data[src.data[i]];
        }
    }


    static void apply_color_table(ImageGray const& src, auto const& dst, Image const& table)
    {
        apply_color_table(img::to_span(src), to_span(dst), img::to_span(table));
    }
    
    
    static void apply_color_table_pma(Span8 const& src, Span32 const& dst, Span32 const& table, f32 alpha)
    {
        f32 r = 0.0f;
        f32 g = 0.0f;
        f32 b = 0.0f;

        p32 ps;

        for (u32 i = 0; i < src.length; i++)
        {
            ps = table.data[src.data[i]];

            r = ps.red * alpha + 0.5f;
            g = ps.green * alpha + 0.5f;
            b = ps.blue * alpha + 0.5f;

            dst.data[i] = img::to_pixel((u8)r, (u8)g, (u8)b);
        }
    }
    
    
    static ByteView make_byte_view(MemoryBuffer<u8> const& buffer, ImageInfo const& info)
    {
        ByteView view{};

        view.data = buffer.data_ + info.offset;
        view.length = info.size;

        return view;
    }


    static bool load_image_asset(AssetData const& src, Image& dst, ImageInfo const& info)
    {
        using FT = bt::FileType;

        bool ok = true;

        auto view = make_byte_view(src.bytes, info);        

        ok &= img::read_image_from_memory(view, dst);
        app_assert(ok && "*** Read image error ***");

        ok &= dst.width == info.width;
        ok &= dst.height == info.height;
        app_assert(ok && "*** Image dimensions mismatch ***");

        return ok;
    }


    static bool load_image_asset(AssetData const& src, ImageGray& dst, ImageInfo const& info)
    {
        using FT = bt::FileType;

        bool ok = true;

        auto view = make_byte_view(src.bytes, info);

        ok &= img::read_image_from_memory(view, dst);
        app_assert(ok && "*** Read image error ***");

        ok &= dst.width == info.width;
        ok &= dst.height == info.height;
        app_assert(ok && "*** Image dimensions mismatch ***");

        return ok;
    }


    static bool load_sky_base_image(AssetData const& src, BackgroundView const& dst, ImageInfo const& info)
    {
        Image part_yx;
        bool ok = load_image_asset(src, part_yx, info);
        if (!ok)
        {
            return false;
        }

        auto& dims = dst.dims.proc;

        ok &= part_yx.width == dims.width;
        ok &= dims.height % part_yx.height == 0;
        app_assert(ok && "*** Unexpected image part size ***");

        if (!ok)
        {
            img::destroy_image(part_yx);
            return false;
        }

        // pre-multiplied alpha
        auto span_part = img::to_span(part_yx);
        apply_pma(span_part, span_part, SKY_BASE_ALPHA);

        extend_view_y(part_yx, to_image_view(dst));
        img::destroy_image(part_yx);

        return true;
    }


    static bool load_sky_overlay_image(AssetData const& src, SkyOverlayView const& dst, ImageInfo const& ov, ImageInfo const& ct)
    {
        ImageGray overlay;
        bool ok = load_image_asset(src, overlay, ov);
        if (!ok)
        {
            return false;
        }
        
        ok &= overlay.width == dst.width;
        ok &= overlay.height == dst.height;
        app_assert(ok && "*** Unexpected overlay image size ***");

        Image table;
        ok = load_image_asset(src, table, ct);
        if (!ok)
        {
            return false;
        }

        ok &= table.width == 256;
        ok &= table.height = 1;
        app_assert(ok && "*** Unexpected color table image size ***");

        if (!ok)
        {
            img::destroy_image(overlay);
            img::destroy_image(table);
            return false;
        }

        // pre-multiplied alpha
        auto s = img::to_span(overlay);
        auto d = to_span(dst);
        auto t = img::to_span(table);
        apply_color_table_pma(s, d, t, SKY_OVERLAY_ALPHA);

        img::destroy_image(overlay);
        img::destroy_image(table);

        return true;
    }


    static bool load_background_image(AssetData const& src, BackgroundView const& dst, ImageInfo const& info)
    {
        ImageGray mask_yx;
        bool ok = load_image_asset(src, mask_yx, info);
        if (!ok)
        {
            return false;
        }

        auto& dims = dst.dims.proc;

        ok &= mask_yx.width == dims.width;
        ok &= mask_yx.height == dims.height;
        app_assert(ok && "*** Unexpected background size ***");

        if (!ok)
        {
            return false;
        }

        mask_convert(img::to_span(mask_yx), to_span(dst));

        img::destroy_image(mask_yx);

        return true;
    }


    static bool load_spritesheet_image(AssetData const& src, SpritesheetView const& dst, ImageInfo const& info)
    {
        bool ok = true;
        auto dims = dst.dims.proc;

        ok &= dims.width == info.width;
        ok &= dims.height == info.height;
        app_assert(ok && "*** Spritesheet dimensions mismatch ***");

        if (!ok)
        {
            return false;
        }
        
        Image ss_image;

        ok = load_image_asset(src, ss_image, info);
        ok &= ss_image.width == info.width;
        ok &= ss_image.height == info.height;
        app_assert(ok && "*** spritesheet asset dimensions mismatch ***");

        ok &= ss_image.height > ss_image.width;
        ok &= ss_image.height % ss_image.width == 0;
        app_assert(ok && "*** Invalid spritesheet asset dimensions ***");

        span::copy(img::to_span(ss_image), to_span(dst));
        img::destroy_image(ss_image);

        return ok;
    }
}
}


/* backgrounds */

namespace game_punk
{
namespace assets
{  
    static bool load_sky_base(AssetData const& src, BackgroundView const& dst)
    {
        using FT = bt::FileType;

        constexpr bt::InfoList_Image_Sky_Base list;
        constexpr auto file_type_ok = list.file_type == FT::Image4C;

        static_assert(file_type_ok && "*** RGBA image expected ***");

        auto& info = list.file_info.base_day_png;

        auto ok = load_sky_base_image(src, dst, info);
        if (!ok)
        {
            return false;
        }

        return true;
    }


    static bool load_sky_overlay(AssetData const& src, SkyOverlayView const& dst)
    {
        using FT = bt::FileType;

        constexpr bt::InfoList_Image_Sky_Overlay list;
        constexpr auto file_type_ok = list.file_type == FT::Image1C;        

        constexpr bt::InfoList_Image_Sky_ColorTable tables;
        constexpr auto table_ok = tables.file_type == FT::Image4C;

        static_assert(file_type_ok && "*** Gray image expected ***");
        static_assert(table_ok && "*** RGBA image expected ***");

        auto& ov = list.file_info.ov_13_png;
        auto& ct = tables.file_info.ct_13_png;

        auto ok = load_sky_overlay_image(src, dst, ov, ct);
        if (!ok)
        {
            return false;
        }

        return true;
    }


    static bool load_background_1(AssetData const& src, BackgroundView const& dst)
    {
        using FT = bt::FileType;

        constexpr bt::Background_Bg1 list;
        constexpr auto file_type_ok = list.file_type == FT::Image1C;

        static_assert(file_type_ok && "*** 1 channel image expected ***");

        auto& info = list.file_info.B;

        auto ok = load_background_image(src, dst, info);

        Image table;
        load_image_asset(src, table, list.color_table);

        auto color = img::pixel_at(img::make_view(table), 8, 0);

        mask_fill(to_span(dst), color);

        img::destroy_image(table);

        return ok;
    }


    static bool load_background_2(AssetData const& src, BackgroundView const& dst)
    {
        using FT = bt::FileType;

        constexpr bt::Background_Bg2 list;
        constexpr auto file_type_ok = list.file_type == FT::Image1C;

        static_assert(file_type_ok && "*** 1 channel image expected ***");

        auto& info = list.file_info.B;

        auto ok = load_background_image(src, dst, info);

        Image table;
        load_image_asset(src, table, list.color_table);

        auto color = img::pixel_at(img::make_view(table), 6, 0);

        game_punk::mask_fill(to_span(dst), color);

        img::destroy_image(table);

        return ok;
    }

    
} // assets
    
}


/* spritesheet */

namespace game_punk
{
namespace assets
{
    
}
}


/* tiles */

namespace game_punk
{
namespace assets
{
    
}
}


/* ui */

namespace game_punk
{
namespace assets
{
    static bool load_ui_font(AssetData const& src, UIState& ui)
    {
        using FT = bt::FileType;

        bt::UIset_Font font;

        constexpr auto file_type_ok = font.file_type == FT::Image1C;
        constexpr auto table_type_ok = font.table_type == FT::Image4C;

        static_assert(file_type_ok && "*** Grayscale image expected ***");
        static_assert(table_type_ok && "*** RGBA image expected ***");

        static_assert(UIState::CTS == font.color_table.width);

        bool ok = true;

        Image table;
        ImageGray mask;
        ok &= load_image_asset(src, table, font.color_table);
        ok &= load_image_asset(src, mask, font.file_info.font);

        if (ok)
        {
            for (u32 i = 0; i < ui.CTS; i++)
            {
                ui.data.colors[i] = table.data_[i];
            }
            
            mask_convert(mask, to_image_view(ui.data.font));
        }

        img::destroy_image(table);
        img::destroy_image(mask);

        return ok;
    }


    static bool load_ui_title(AssetData const& src, UIState& ui)
    {
        using FT = bt::FileType;

        bt::UIset_Title title;

        constexpr auto file_type_ok = title.file_type == FT::Image1C;
        constexpr auto table_type_ok = title.table_type == FT::Image4C;

        static_assert(file_type_ok && "*** Grayscale image expected ***");
        static_assert(table_type_ok && "*** RGBA image expected ***");

        bool ok = true;

        Image table;
        ImageGray mask;
        ok &= load_image_asset(src, table, title.color_table);
        ok &= load_image_asset(src, mask, title.file_info.title_main);

        if (ok)
        {
            apply_color_table(mask, ui.data.title, table);
        }

        img::destroy_image(table);
        img::destroy_image(mask);

        return ok;
    }


    static bool load_ui_icons(AssetData const& src, UIState& ui)
    {
        using FT = bt::FileType;

        bt::UIset_Icons icons;

        constexpr auto file_type_ok = icons.file_type == FT::Image1C;
        constexpr auto table_type_ok = icons.table_type == FT::Image4C;

        static_assert(file_type_ok && "*** Grayscale image expected ***");
        static_assert(table_type_ok && "*** RGBA image expected ***");

        bool ok = true;

        ImageGray filter;

        ok &= load_image_asset(src, filter, icons.file_info.icons);

        if (ok)
        {
            filter_convert(filter, to_image_view(ui.data.icons));
        }

        img::destroy_image(filter);

        return ok;
    }
}
}


/* set game data */

namespace game_punk
{
namespace assets
{  
    static bool load_background_assets(AssetData const& src, BackgroundState& bg_state)
    {
        bool ok = true;

        ok &= load_sky_base(src, bg_state.sky.base);
        ok &= load_sky_overlay(src, bg_state.sky.overlay_src);
        ok &= load_background_1(src, bg_state.bg_1.data[0]);
        ok &= load_background_2(src, bg_state.bg_2.data[0]);

        render_front_back(bg_state.sky);

        return ok;
    }


    static bool load_spritesheet_assets(AssetData const& src, SpritesheetState& ss_state)
    {       
        using FT = bt::FileType;

        bt::Spriteset_Punk list;

        constexpr auto tile_type_ok = list.file_type == FT::Image1C;
        constexpr auto table_type_ok = list.table_type == FT::Image4C;

        static_assert(tile_type_ok && "*** Grayscale image expected ***");
        static_assert(table_type_ok && "*** RGBA image expected ***");

        bool ok = true;

        Image table;
        ImageGray spritesheet;

        ok &= load_image_asset(src, table, list.color_table);

        ok &= load_image_asset(src, spritesheet, list.file_info.Punk_run);
        apply_color_table(spritesheet, ss_state.punk_run, table);
        img::destroy_image(spritesheet);

        img::destroy_image(table);

        return ok;
    }


    static bool load_tile_assets(AssetData const& src, TileState& tile_state)
    {
        using FT = bt::FileType;

        bt::Tileset_ex_zone list;

        constexpr auto tile_type_ok = list.file_type == FT::Image1C;
        constexpr auto table_type_ok = list.table_type == FT::Image4C;

        static_assert(tile_type_ok && "*** Grayscale image expected ***");
        static_assert(table_type_ok && "*** RGBA image expected ***");

        bool ok = true;

        Image table;
        ImageGray tile;

        ok &= load_image_asset(src, table, list.color_table);
        
        ok &= load_image_asset(src, tile, list.file_info.floor_02);
        apply_color_table(tile, tile_state.floor_a, table);
        img::destroy_image(tile);

        ok &= load_image_asset(src, tile, list.file_info.floor_03);
        apply_color_table(tile, tile_state.floor_b, table);
        img::destroy_image(tile);

        img::destroy_image(table);

        return ok;
    }


    static bool load_ui_assets(AssetData const& src, UIState& ui)
    {
        bool ok = true;

        ok &= load_ui_font(src, ui);
        ok &= load_ui_title(src, ui);
        ok &= load_ui_icons(src, ui);

        return ok;
    }


    static void read_game_assets(StateData& data)
    {
        auto& src = data.asset_data;

        bool ok = true;
        ok &= load_background_assets(src, data.background);
        ok &= load_spritesheet_assets(src, data.spritesheet);
        ok &= load_tile_assets(src, data.tiles);
        ok &= load_ui_assets(src, data.ui);

        app_assert(ok && "*** Error reading asset data ***");

        src.status = ok ? AssetStatus::Success : AssetStatus::FailRead;
    }
}


}


//#define GAME_PUNK_WASM


#if defined(__EMSCRIPTEN__) || defined(GAME_PUNK_WASM)

#include <emscripten/fetch.h>
#include <string.h>

#endif


/* asset data */

namespace game_punk
{
namespace assets
{
#if defined(__EMSCRIPTEN__) || defined(GAME_PUNK_WASM)


    constexpr auto GAME_DATA_PATH = "https://raw.githubusercontent.com/adam-lafontaine/CMS/punk-run-v0.1.0/sm/wasm/punk_run.bin"; // almostalwaysadam.com

    //constexpr auto GAME_DATA_PATH = "./punk_run.bin"; // itch.io

    namespace em_load
    {
        using FetchAttr = emscripten_fetch_attr_t;
        using FetchResponse = emscripten_fetch_t;


        static ByteView make_byte_view(FetchResponse* res)
        {
            ByteView bytes;
            bytes.data = (u8*)res->data;
            bytes.length = res->numBytes;

            return bytes;
        }


        static void fetch_bin_data_fail(FetchResponse* res)
        {                
            auto& data = *(StateData*)(res->userData);

            data.asset_data.bin_file_path = 0;
            data.asset_data.status = AssetStatus::FailLoad;
            
            emscripten_fetch_close(res);
        }


        static void fetch_bin_data_success(FetchResponse* res)
        {
            auto& data = *(StateData*)(res->userData);

            auto bytes = make_byte_view(res);

            auto& buffer = data.asset_data.bytes;
            if (!mb::create_buffer(buffer, bytes.length, fs::get_file_name(res->url)))
            {
                emscripten_fetch_close(res);
                return;
            }

            span::copy(bytes, span::make_view(buffer));

            emscripten_fetch_close(res);

            read_game_assets(data);
        }


        static void fetch_bin_data_async(cstr url, StateData& data)
        {            
            FetchAttr attr;
            emscripten_fetch_attr_init(&attr);
            //stb::qsnprintf(attr.requestMethod, 4, "GET");
            strcpy(attr.requestMethod, "GET");
            attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
            attr.userData = (void*)&data;
            attr.onsuccess = fetch_bin_data_success;
            attr.onerror = fetch_bin_data_fail;

            emscripten_fetch(&attr, url);
        }

    }


    static void load_game_assets(StateData& data)
    {   
        data.asset_data.status = AssetStatus::Loading;
        em_load::fetch_bin_data_async(GAME_DATA_PATH, data);
    }

#else

    constexpr auto GAME_DATA_PATH = "./punk_run.bin";


#ifdef _WIN32
    constexpr auto GAME_DATA_PATH_FALLBACK = "C:/D_Data/Repos/GamePunkRun/game_punk/res/xbin/punk_run.bin";
#else
    constexpr auto GAME_DATA_PATH_FALLBACK = "/home/adam/Repos/GamePunkRun/game_punk/res/xbin/punk_run.bin";
#endif   


    static bool load_asset_data(AssetData& dst)
    {
        cstr path = dst.bin_file_path;
        auto& buffer = dst.bytes;

        if (path)
        {
            buffer = fs::read_bytes(path);
            if (buffer.ok)
            {
                return true;
            }
        }

        path = GAME_DATA_PATH;
        buffer = fs::read_bytes(path);
        if (buffer.ok)
        {
            dst.bin_file_path = path;
            return true;
        }

        path = GAME_DATA_PATH_FALLBACK;
        buffer = fs::read_bytes(path);
        if (buffer.ok)
        {
            dst.bin_file_path = path;
            return true;
        }
        
        return false;
    }


    static void load_game_assets(StateData& data)
    {        
        data.asset_data.status = AssetStatus::Loading;

        bool ok = true;
        ok &= load_asset_data(data.asset_data);

        app_assert(ok && "*** Error loading asset data ***");

        if (!ok)
        {
            data.asset_data.status = AssetStatus::FailLoad;
            return;
        }

        read_game_assets(data);
    }

#endif
}
}


