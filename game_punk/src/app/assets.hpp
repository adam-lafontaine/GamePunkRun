#pragma once

#include "../../../libs/io/filesystem.hpp"


/* definitions */

namespace game_punk
{
namespace assets
{
    template <u8 FT>
    using ImageInfo = bt::FileInfo_Image<FT>;

    using SkyBaseInfo = bt::InfoList_Image_Sky_Base::FileInfo;
    using SkyOverlayInfo = bt::InfoList_Image_Sky_Overlay::FileInfo;
    using SkyTableInfo = bt::InfoList_Image_Sky_ColorTable::FileInfo;


    constexpr f32 SKY_OVERLAY_ALPHA = 0.22f;
    constexpr f32 SKY_BASE_ALPHA = 1.0f - SKY_OVERLAY_ALPHA;

}
}


/* helpers */

namespace game_punk
{

/* read wrappers */

namespace assets
{
    static bool read_result(bt::ReadResult res)
    {
        using RR = bt::ReadResult;

        switch (res)
        {
        case RR::OK: return true;

        case RR::Unsupported:
            app_crash("*** Read asset unsupported ***");
            break;

        case RR::ReadError:
            app_crash("*** Read asset error ***");
            break;

        case RR::SizeError:
            app_crash("*** Read asset size error ***");
            break;

        default:
            break;
        }

        return false;
    }


    static bool read_rgba(AssetData const& src, bt::ImageRGBAInfo const& info, Image& dst)
    {
        return read_result(bt::read_rgba(src.bytes, info, dst));
    }


    static bool read_color_table(AssetData const& src, bt::ColorTableInfo const& info, bt::ColorTable4C& dst)
    {
        return read_result(bt::read_color_table(src.bytes, info, dst));
    }


    static bool read_table_image(AssetData const& src, bt::TableImageInfo const& info, bt::TableImage1C& dst)
    {
        return read_result(bt::read_table_image(src.bytes, info, dst));
    }


    static bool read_filter_image(AssetData const& src, bt::FilterImageInfo const& info, bt::FilterImage1C& dst)
    {
        return read_result(bt::read_filter_image(src.bytes, info, dst));
    }


    static bool read_mask_image(AssetData const& src, bt::MaskImageInfo const& info, bt::MaskImage1C& dst)
    {
        return read_result(bt::read_mask_image(src.bytes, info, dst));
    }
}


/* tests */

namespace assets
{    
    template <u8 uFT, class IMG>
    static bool test_read_images(AssetData const& src, SpanView<bt::FileInfo_Image<uFT>> const& file_info, IMG& dst)
    {
        constexpr auto type = (bt::FileType)uFT;
        static_assert(bt::data_size(type) == sizeof(dst.data_[0]));

        for (u32 i = 0; i < file_info.length; i++)
        {
            auto& info = file_info.data[i];
            auto data = make_byte_view(src.bytes, info);
            auto ok = bt::read_image(data, info.type, dst) == bt::ReadResult::OK;
            img::destroy_image(dst);
            if (!ok)
            {
                return false;
            }
        }

        return true;
    }
}


namespace assets
{   
    
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


    static bool apply_color_table_pma(bt::TableImage1C const& src, ImageView const& dst, bt::ColorTable4C const& table, f32 alpha)
    {
        if (src.width != dst.width || src.height != dst.height)
		{
			return false;
		}

        f32 r = 0.0f;
        f32 g = 0.0f;
        f32 b = 0.0f;

        p32 ps;

        auto length = src.width * src.height;
        auto s = src.data;
        auto d = dst.matrix_data_;
        auto t = table.data;

        for (u32 i = 0; i < length; i++)
        {
            ps = t[s[i]];

            r = ps.red * alpha + 0.5f;
            g = ps.green * alpha + 0.5f;
            b = ps.blue * alpha + 0.5f;

            d[i] = img::to_pixel((u8)r, (u8)g, (u8)b);
        }

        return true;
    }    
    

    static bool load_table_image(AssetData const& src, ImageView const& dst, bt::TableImageInfo const& info, bt::ColorTableInfo const& ct)
    {
        bt::TableImage1C table_image;
        bool ok = read_table_image(src, info, table_image);
        if (!ok)
        {
            return false;
        }

        ok &= table_image.width == info.width;
        ok &= table_image.height == info.height;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected image size ***");
            bt::destroy_image(table_image);
        }

        bt::ColorTable4C table;
        ok &= read_color_table(src, ct, table);
        if (!ok)
        {
            bt::destroy_image(table_image);
            return false;
        }

        ok &= table.length == 256;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected color table image size ***");
            bt::destroy_image(table_image);
            bt::destroy_image(table);
            return false;
        }

        ok &= bt::color_table_convert(table_image, table, dst);
        if (!ok)
        {
            app_assert(ok && "*** color_table_convert() ***");
            bt::destroy_image(table_image);
            bt::destroy_image(table);
            return false;
        }

        bt::destroy_image(table_image);
        bt::destroy_image(table);
        
        return true;
    }


    static bool load_filter_image(AssetData const& src, ImageView const& dst, bt::FilterImageInfo const& info)
    {
        bt::FilterImage1C filter;
        bool ok = read_filter_image(src, info, filter);
        if (!ok)
        {
            return false;
        }

        ok &= filter.width == info.width;
        ok &= filter.height == info.height;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected image size ***");
            bt::destroy_image(filter);
            return false;
        }

        ok &= bt::filter_convert(filter, dst);
        if (!ok)
        {
            app_assert(ok && "*** filter_convert() ***");
            bt::destroy_image(filter);
            return false;
        }

        bt::destroy_image(filter);

        return ok;
    }
}
}


/* backgrounds */

namespace game_punk
{
namespace assets
{  
    static bool load_sky_base_image(AssetData const& src, BackgroundView const& dst, SkyBaseInfo const& info)
    {
        Image part_yx;

        bool ok = read_rgba(src, info, part_yx);
        if (!ok)
        {
            return false;
        }

        auto& dims = dst.dims.proc;

        ok &= part_yx.width == dims.width;
        ok &= dims.height % part_yx.height == 0;        

        if (!ok)
        {
            app_assert(ok && "*** Unexpected image part size ***");
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


    static bool load_sky_overlay_image(AssetData const& src, SkyOverlayView const& dst, SkyOverlayInfo const& ov, SkyTableInfo const& ct)
    {
        bt::TableImage1C overlay;
        bool ok = read_table_image(src, ov, overlay);
        if (!ok)
        {
            return false;
        }
        
        ok &= overlay.width == dst.width;
        ok &= overlay.height == dst.height;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected overlay image size ***");
            bt::destroy_image(overlay);
            return false;
        }        

        bt::ColorTable4C table;
        ok = read_color_table(src, ct, table);
        if (!ok)
        {
            bt::destroy_image(overlay);
            return false;
        }

        ok &= table.length == 256;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected color table image size ***");
            bt::destroy_image(overlay);
            bt::destroy_image(table);
            return false;
        }

        // pre-multiplied alpha
        ok &= apply_color_table_pma(overlay, to_image_view(dst), table, SKY_OVERLAY_ALPHA);
        if (!ok)
        {
            app_assert(ok && "*** apply_color_table_pma() ***");
            bt::destroy_image(overlay);
            bt::destroy_image(table);            
            return false;
        }

        bt::destroy_image(overlay);
        bt::destroy_image(table);

        return ok;
    }


    static bool load_background_mask(AssetData const& src, BackgroundView const& dst, bt::MaskImageInfo const& info)
    {
        bt::MaskImage1C mask;
        bool ok = read_mask_image(src, info, mask);
        if (!ok)
        {
            return false;
        }

        auto& dims = dst.dims.proc;

        ok &= mask.width == dims.width;
        ok &= mask.height == dims.height;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected background size ***");
            bt::destroy_image(mask);
            return false;
        }

        ok &= bt::mask_convert(mask, to_image_view(dst));
        if (!ok)
        {
            app_assert(ok && "*** mask_convert() ***");
            bt::destroy_image(mask);
            return false;
        }

        bt::destroy_image(mask);

        return true;
    }


    static bool load_sky_base(AssetData const& src, BackgroundView const& dst)
    {
        constexpr bt::InfoList_Image_Sky_Base list;

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
        constexpr bt::InfoList_Image_Sky_ColorTable tables;

        auto& ov = list.file_info.ov_13_png;
        auto& ct = tables.file_info.ct_13_png;

        auto ok = load_sky_overlay_image(src, dst, ov, ct);
        if (!ok)
        {
            return false;
        }

        return true;
    }


    static bool load_background_1(AssetData const& src, BackgroundAnimationFast const& dst)
    {
        constexpr bt::Background_Bg1 list;
        constexpr auto color_id = 8;

        bool ok = dst.count == list.count;
        if (!ok)
        {
            app_assert(ok && "*** BackgroundAnimation not initialized ***");
            return false;
        }

        for (u32 i = 0; i < list.count; i++)
        {
            ok &= has_data(dst.data[i]);
        }

        if (!ok)
        {
            app_assert(ok && "*** BackgroundAnimation not created ***");
            return false;
        }

        bt::ColorTable4C table;
        ok &= read_color_table(src, list.color_table, table);

        auto color = table.data[color_id];

        for (u32 i = 0; i < list.count; i++)
        {
            auto& view = dst.data[i];
            ok &= load_background_mask(src, view, list.items[i]);
            if (ok)
            {
                bt::mask_update(to_image_view(view), color);
            }            
        }

        bt::destroy_image(table);

        return ok;
    }


    static bool load_background_2(AssetData const& src, BackgroundAnimationFast const& dst)
    {
        constexpr bt::Background_Bg2 list;
        constexpr auto color_id = 6;

        bool ok = dst.count == list.count;
        if (!ok)
        {
            app_assert(ok && "*** BackgroundAnimation not initialized ***");
            return false;
        }

        for (u32 i = 0; i < list.count; i++)
        {
            ok &= has_data(dst.data[i]);
        }

        if (!ok)
        {
            app_assert(ok && "*** BackgroundAnimation not created ***");
            return false;
        }

        bt::ColorTable4C table;
        ok &= read_color_table(src, list.color_table, table);
        if (!ok)
        {
            return false;
        }

        ok &= table.length == 256;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected color table image size ***");
            bt::destroy_image(table);
            return false;
        }

        auto color = table.data[color_id];
        bt::destroy_image(table);

        for (u32 i = 0; i < list.count; i++)
        {
            auto& view = dst.data[i];
            ok &= load_background_mask(src, view, list.items[i]);
            if (ok)
            {
                bt::mask_update(to_image_view(view), color);
            }            
        }

        

        return ok;
    }


    static bool load_background_1(AssetData const& src, BackgroundAnimation& dst)
    {
        constexpr bt::Background_Bg1 list;
        constexpr auto color_id = 8;

        dst.asset_count = list.count;
        dst.select_count = dst.asset_count - dst.work_count;

        for (u32 i = 0; i < list.count; i++)
        {
            dst.asset_info[i] = list.items[i];
        }

        bool ok = true;

        ok &= has_data(dst.table);

        for (u32 i = 0; i < dst.data_count; i++)
        {
            ok &= has_data(dst.data[i]);
        }

        if (!ok)
        {
            app_assert(ok && "*** BackgroundAnimation not created ***");
            return false;
        }

        bt::ColorTable4C table;
        ok &= read_color_table(src, list.color_table, table);
        if (!ok)
        {
            return false;
        }

        ok &= table.length == 256;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected color table image size ***");
            bt::destroy_image(table);
            return false;
        }

        for (u32 i = 0; i < table.length; i++)
        {
            dst.table.data[i] = table.data[i];
        }
        bt::destroy_image(table);        

        auto color = table.data[color_id];

        for (u32 i = 0; i < dst.data_count; i++)
        {
            auto& view = dst.data[i];
            ok &= load_background_mask(src, view, dst.asset_info[i]);
            if (ok)
            {
                bt::mask_update(to_image_view(view), color);
            }
        }        

        return ok;
    }


    static bool load_background_2(AssetData const& src, BackgroundAnimation& dst)
    {
        constexpr bt::Background_Bg2 list;
        constexpr auto color_id = 6;

        dst.asset_count = list.count;
        dst.select_count = dst.asset_count - dst.work_count;

        for (u32 i = 0; i < list.count; i++)
        {
            dst.asset_info[i] = list.items[i];
        }

        bool ok = true;

        ok &= has_data(dst.table);

        for (u32 i = 0; i < dst.data_count; i++)
        {
            ok &= has_data(dst.data[i]);
        }

        if (!ok)
        {
            app_assert(ok && "*** BackgroundAnimation not created ***");
            return false;
        }

        bt::ColorTable4C table;
        ok &= read_color_table(src, list.color_table, table);
        if (!ok)
        {
            return false;
        }

        ok &= table.length == 256;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected color table image size ***");
            bt::destroy_image(table);
            return false;
        }

        for (u32 i = 0; i < table.length; i++)
        {
            dst.table.data[i] = table.data[i];
        }
        bt::destroy_image(table);        

        auto color = table.data[color_id];

        for (u32 i = 0; i < dst.data_count; i++)
        {
            auto& view = dst.data[i];
            ok &= load_background_mask(src, view, dst.asset_info[i]);
            if (ok)
            {
                bt::mask_update(to_image_view(view), color);
            }
        }        

        return ok;
    }

    
} // assets
    
}


/* spritesheet */

namespace game_punk
{
namespace assets
{
    static bool load_spritesheet_image(AssetData const& src, SpritesheetView const& dst, bt::TableImageInfo const& ss, bt::ColorTableInfo const& ct)
    {
        return load_table_image(src, to_image_view(dst), ss, ct);
    }


    static bool load_sprites_punk(AssetData const& src, SpritesheetState const& ss_state)
    {
        bt::Spriteset_Punk list;

        auto& info = list.file_info.Punk_run;
        auto& table = list.color_table;

        return load_spritesheet_image(src, ss_state.punk_run, info, table);
    }
}
}


/* tiles */

namespace game_punk
{
namespace assets
{
    static bool load_tile_image(AssetData const& src, TileView const& dst, bt::TableImageInfo const& info, bt::ColorTableInfo const& ct)
    {
        return load_table_image(src, to_image_view(dst), info, ct);
    }


    static bool load_tiles_ex_zone(AssetData const& src, TileState const& tiles)
    {
        bt::Tileset_ex_zone list;

        auto& floor2 = list.file_info.floor_02;
        auto& floor3 = list.file_info.floor_03;
        auto& table = list.color_table;

        bool ok = true;

        ok &= load_tile_image(src, tiles.floor_a, floor2, table);
        ok &= load_tile_image(src, tiles.floor_b, floor3, table);

        return ok;
    }
}
}


/* ui */

namespace game_punk
{
namespace assets
{
    static bool load_ui_font(AssetData const& src, UIState& ui)
    {
        bt::UIset_Font list;

        auto& font = list.file_info.font;

        return load_filter_image(src, to_image_view(ui.data.font), font);        
    }


    static bool load_ui_title(AssetData const& src, UIState& ui)
    {
        bt::UIset_Title list;

        auto& title = list.file_info.title_main;

        return load_filter_image(src, ui.data.title, title);
    }


    static bool load_ui_icons(AssetData const& src, UIState& ui)
    {
        bt::UIset_Icons list;

        auto& icons = list.file_info.icons;

        return load_filter_image(src, to_image_view(ui.data.icons), icons);
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
        ok &= load_background_1(src, bg_state.bg_1);
        ok &= load_background_2(src, bg_state.bg_2);

        ok &= load_background_1(src, bg_state.bgf_1);
        ok &= load_background_2(src, bg_state.bgf_2);

        render_front_back(bg_state.sky);

        return ok;
    }


    static bool load_spritesheet_assets(AssetData const& src, SpritesheetState const& ss_state)
    {  
        bool ok = true;

        ok &= load_sprites_punk(src, ss_state);

        return ok;
    }


    static bool load_tile_assets(AssetData const& src, TileState& tile_state)
    {
        bool ok = true;

        ok &= load_tiles_ex_zone(src, tile_state);

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


    static bool test_game_assets(AssetData const& src)
    {
        bool ok = true;

        ok &= bt::InfoList_Image_Sky_Base().test(src.bytes);
        ok &= bt::InfoList_Image_Sky_Overlay().test(src.bytes);
        ok &= bt::InfoList_Image_Sky_ColorTable().test(src.bytes);
        ok &= bt::Background_Bg1().test(src.bytes);
        ok &= bt::Background_Bg2().test(src.bytes);
        ok &= bt::Spriteset_Punk().test(src.bytes);
        ok &= bt::Tileset_ex_zone().test(src.bytes);
        ok &= bt::UIset_Font().test(src.bytes);
        ok &= bt::UIset_Title().test(src.bytes);
        ok &= bt::UIset_Icons().test(src.bytes);

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
        if (!ok)
        {
            app_assert(ok && "*** Error loading asset data ***");
            data.asset_data.status = AssetStatus::FailLoad;
            return;
        }

        ok &= test_game_assets(data.asset_data);
        if (!ok)
        {
            app_assert(ok && "*** Asset tests failed ***");
            data.asset_data.status = AssetStatus::FailLoad;
            return;
        }


        read_game_assets(data);
    }

#endif
}
}


