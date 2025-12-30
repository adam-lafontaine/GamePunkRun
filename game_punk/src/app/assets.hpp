#pragma once

#include "../../../libs/io/filesystem.hpp"


/* definitions */

namespace game_punk
{
namespace assets
{
    
    using ImageInfo = bt::AssetInfo_Image;


    constexpr f32 SKY_OVERLAY_ALPHA = 0.22f;
    constexpr f32 SKY_BASE_ALPHA = 1.0f - SKY_OVERLAY_ALPHA;

}
}


/* helpers */

namespace game_punk
{
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
 

    static bool apply_color_table_pma(bt::TableFilterImage const& src, ImageView const& dst, bt::ColorTableImage const& table, f32 alpha)
    {
        if (src.gray.width != dst.width || src.gray.height != dst.height)
		{
			return false;
		}

        f32 r = 0.0f;
        f32 g = 0.0f;
        f32 b = 0.0f;

        p32 ps;

        auto length = dst.width * dst.height;
        auto s = src.gray.data_;
        auto d = dst.matrix_data_;
        auto t = table.rgba.data_;

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
   

}


/* backgrounds */

namespace assets
{ 
    static bool init_load_sky_base(Buffer8 const& buffer, SkyAnimation& sky)
    {
        using Def = bt::SkyBase_base;
        Def base;        

        Image part_yx = base.read_rgba_item(buffer, Def::Items::day);
        
        auto& dst = sky.base;

        auto& dims = dst.dims.proc;

        bool ok = true;
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


    static bool init_load_sky_overlay(Buffer8 const& buffer, SkyAnimation& sky)
    {
        using Def = bt::SkyOverlay_overlay;

        Def overlay;
        auto item = Def::Items::ov_13;

        auto table = overlay.read_table_item(buffer, item);
        auto filter = overlay.read_table_filter_item(buffer, item);

        auto& dst = sky.overlay_src;

        bool ok = true;
        ok &= filter.gray.width == dst.width;
        ok &= filter.gray.height == dst.height;
        if (!ok)
        {
            app_assert(ok && "*** Unexpected table filter size ***");
            table.destroy();
            filter.destroy();
            return false;
        }

        ok &= bt::color_table_convert(filter, table, to_image_view(dst));

        app_assert(ok && "*** bt::color_table_convert() ***");

        table.destroy();
        filter.destroy();

        return true;
    }


    template <class BG_DEF>
    static bool init_load_background(Buffer8 const& buffer, BackgroundAnimation& bg, u32 color_id)
    {
        using Def = BG_DEF;

        Def def;

        constexpr auto N = sizeof(bg.background_data) / sizeof(bg.background_data[0]);

        static_assert(Def::count >= N);

        auto table = def.read_table(buffer);
        auto color = table.at(color_id);
        bg.load_asset_color = color;
        bg.load_asset_cb = load_background_image<Def>;

        bool ok = true;

        for (u32 i = 0; i < N; i++)
        {
            auto filter = def.read_alpha_filter_item(buffer, (Def::Items)i);
            auto dst = to_image_view(bg.background_data[i]);
            ok &= bt::alpha_filter_convert(filter, dst, color);
            filter.destroy();
        }

        table.destroy();

        return ok;
    }
    
} // assets
    
}


/* spritesheet */

namespace game_punk
{
namespace assets
{
    static bool load_sprites_punk(Buffer8 const& buffer, SpritesheetState const& ss)
    {
        using Punk = bt::Spriteset_Punk;

        Punk list;

        auto table = list.read_table(buffer);
        auto run = list.read_table_filter_item(buffer, Punk::Items::Punk_run);
        auto idle = list.read_table_filter_item(buffer, Punk::Items::Punk_idle);
        
        bool ok = true;

        ok &= bt::color_table_convert(run, table, to_image_view(ss.punk_run));
        ok &= bt::color_table_convert(idle, table, to_image_view(ss.punk_idle));

        table.destroy();
        run.destroy();
        idle.destroy();

        return ok;
    }
}
}


/* tiles */

namespace game_punk
{
namespace assets
{
    static bool load_tiles_ex_zone(Buffer8 const& buffer, TileState const& tiles)
    {
        using Ex = bt::Tileset_ex_zone;

        bt::Tileset_ex_zone list;

        auto table = list.read_table(buffer);
        auto f2 = list.read_table_filter_item(buffer, Ex::Items::floor_02);
        auto f3 = list.read_table_filter_item(buffer, Ex::Items::floor_03);

        bool ok = true;

        ok &= bt::color_table_convert(f2, table, to_image_view(tiles.floor_a));
        ok &= bt::color_table_convert(f3, table, to_image_view(tiles.floor_b));

        table.destroy();
        f2.destroy();
        f3.destroy();

        return ok;
    }
}
}


/* ui */

namespace game_punk
{
namespace assets
{
    static bool load_ui_font(Buffer8 const& buffer, UIState& ui)
    {
        using Font = bt::UIset_Font;

        Font list;

        auto table = list.read_table(buffer);
        auto filter = list.read_table_filter_item(buffer, Font::Items::font);

        bool ok = true;

        ok &= bt::color_table_convert(filter, table, to_image_view(ui.data.font));

        table.destroy();
        filter.destroy();

        return ok;
    }


    static bool load_ui_title(Buffer8 const& buffer, UIState& ui)
    {
        using Title = bt::UIset_Title;

        Title list;

        auto table = list.read_table(buffer);
        auto filter = list.read_table_filter_item(buffer, Title::Items::title_main);

        bool ok = true;

        ok &= bt::color_table_convert(filter, table, to_image_view(ui.data.font));

        table.destroy();
        filter.destroy();

        return ok;
    }


    static bool load_ui_icons(Buffer8 const& buffer, UIState& ui)
    {
        using Icons = bt::UIset_Icons;

        Icons list;

        auto table = list.read_table(buffer);
        auto filter = list.read_table_filter_item(buffer, Icons::Items::icons);

        bool ok = true;

        ok &= bt::color_table_convert(filter, table, to_image_view(ui.data.font));

        table.destroy();
        filter.destroy();

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

        ok &= init_load_sky_base(src.bytes, bg_state.sky);
        ok &= init_load_sky_overlay(src.bytes, bg_state.sky);

        ok &= init_load_background<bt::Background_Bg1>(src.bytes, bg_state.bg_1, 8);
        ok &= init_load_background<bt::Background_Bg2>(src.bytes, bg_state.bg_2, 6);

        render_front_back(bg_state.sky);

        return ok;
    }


    static bool load_spritesheet_assets(AssetData const& src, SpritesheetState const& ss_state)
    {  
        bool ok = true;

        ok &= load_sprites_punk(src.bytes, ss_state);

        return ok;
    }


    static bool load_tile_assets(AssetData const& src, TileState& tile_state)
    {
        bool ok = true;

        ok &= load_tiles_ex_zone(src.bytes, tile_state);

        return ok;
    }


    static bool load_ui_assets(AssetData const& src, UIState& ui)
    {
        bool ok = true;

        ok &= load_ui_font(src.bytes, ui);
        ok &= load_ui_title(src.bytes, ui);
        ok &= load_ui_icons(src.bytes, ui);

        return ok;
    }


    static bool test_game_assets(AssetData const& src)
    {
        static_assert(bt::CLASS_COUNT == 9); // will fail as classes are added/removed

        u32 test_count = 0;

        test_count += (u32)bt::SkyBase_base().test(src.bytes);
        test_count += (u32)bt::SkyOverlay_overlay().test(src.bytes);
        test_count += (u32)bt::Background_Bg1().test(src.bytes);
        test_count += (u32)bt::Background_Bg2().test(src.bytes);
        test_count += (u32)bt::Spriteset_Punk().test(src.bytes);
        test_count += (u32)bt::Tileset_ex_zone().test(src.bytes);
        test_count += (u32)bt::UIset_Font().test(src.bytes);
        test_count += (u32)bt::UIset_Title().test(src.bytes);
        test_count += (u32)bt::UIset_Icons().test(src.bytes);

        return test_count == bt::CLASS_COUNT;
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

#define GAME_PUNK_ASSETS_WEB

#include <emscripten/fetch.h>
#include <string.h>

#endif


/* asset data web */

namespace game_punk
{
namespace assets
{
#ifdef GAME_PUNK_ASSETS_WEB


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

            bool ok &= test_game_assets(data.asset_data);
            if (!ok)
            {
                app_assert(ok && "*** Asset tests failed ***");
                data.asset_data.status = AssetStatus::FailRead;
                return;
            }
            
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

#endif
}
}



/* asset data */

namespace game_punk
{
namespace assets
{
#ifndef GAME_PUNK_ASSETS_WEB

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
            data.asset_data.status = AssetStatus::FailRead;
            return;
        }

        read_game_assets(data);
    }

#endif
}
}