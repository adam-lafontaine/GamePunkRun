#pragma once

#include "../../../libs/io/filesystem.hpp"
#include "../../../libs/datetime/datetime.hpp"


/* definitions */

namespace game_punk
{
namespace assets
{
    namespace dt = datetime;
    
    using ImageInfo = bt::AssetInfo_Image;


    constexpr f32 SKY_OVERLAY_ALPHA = 0.3f;
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
 

    static bool apply_color_table_pma(bt::TableFilterImage const& src, bt::ColorTableImage const& table, ImageView const& dst, f32 alpha)
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

        app_assert(table.rgba.data_);

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

        ok &= apply_color_table_pma(filter, table, to_image_view(dst), SKY_OVERLAY_ALPHA);

        app_assert(ok && "*** bt::color_table_convert() ***");

        table.destroy();
        filter.destroy();

        return true;
    }


    template <typename BG_DEF>
    static bool init_load_background(Buffer8 const& buffer, BackgroundAnimation& bg, u32 color_id)
    {
        //using BG_DEF = bt::Background_Bg1;

        BG_DEF list;

        constexpr auto N = sizeof(bg.background_data) / sizeof(bg.background_data[0]);

        static_assert(BG_DEF::count >= N);

        auto table = list.read_table(buffer);
        auto color = table.at(color_id);

        bg.select_asset_ids.size = BG_DEF::count - bg.work_asset_ids.count;        
        bg.load_cmd.on_load = load_background_image<BG_DEF>;
        bg.load_cmd.ctx.color = color;

        bool ok = true;

        for (u32 i = 0; i < N; i++)
        {
            auto item = static_cast<BG_DEF::Items>(i);
            auto filter = list.read_alpha_filter_item(buffer, item);
            auto dst = to_image_view(bg.background_data[i]);
            ok &= bt::alpha_filter_convert(filter, dst, color);
            app_assert(ok && "*** bt::alpha_filter_convert() ***");
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
        app_assert(ok && "*** bt::color_table_convert() ***");
        ok &= bt::color_table_convert(idle, table, to_image_view(ss.punk_idle));
        app_assert(ok && "*** bt::color_table_convert() ***");

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
        app_assert(ok && "*** bt::color_table_convert() ***");
        ok &= bt::color_table_convert(f3, table, to_image_view(tiles.floor_b));
        app_assert(ok && "*** bt::color_table_convert() ***");

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
        app_assert(ok && "*** bt::color_table_convert() ***");

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

        ok &= bt::color_table_convert(filter, table, to_image_view(ui.data.icons));
        app_assert(ok && "*** bt::color_table_convert() ***");

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
        render_front_back(bg_state.sky);

        ok &= init_load_background<bt::Background_Bg1>(src.bytes, bg_state.bg_1, 8);
        ok &= init_load_background<bt::Background_Bg2>(src.bytes, bg_state.bg_2, 6);        

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
        ok &= load_ui_icons(src.bytes, ui);

        return ok;
    }


    static bool check_asset_version(AssetData const& src)
    {
        return bt::read_version_number(src.bytes) == bt::VERSION;
    }


    static bool test_game_assets(AssetData const& src)
    {
        static_assert(bt::CLASS_COUNT == 8); // will fail as classes are added/removed

        u32 test_count = 0;
        
        test_count += (u32)bt::SkyBase_base().test(src.bytes);
        test_count += (u32)bt::SkyOverlay_overlay().test(src.bytes);
        test_count += (u32)bt::Background_Bg1().test(src.bytes);
        test_count += (u32)bt::Background_Bg2().test(src.bytes);
        test_count += (u32)bt::Spriteset_Punk().test(src.bytes);
        test_count += (u32)bt::Tileset_ex_zone().test(src.bytes);
        test_count += (u32)bt::UIset_Font().test(src.bytes);
        test_count += (u32)bt::UIset_Icons().test(src.bytes);

        auto res = test_count == bt::CLASS_COUNT;

        return res;
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


#define GAME_PUNK_EDITING_WASM

#ifdef GAME_PUNK_EDITING_WASM
#ifndef GAME_PUNK_WASM
#define GAME_PUNK_WASM
#endif
#endif


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

    // itch.io
    constexpr auto GAME_DATA_PATH_LOCAL = "./punk_run.bin";

    // almostalwaysadam.com
    constexpr auto GAME_DATA_PATH_CMS = "https://raw.githubusercontent.com/adam-lafontaine/CMS/sm-current/sm/wasm/punk_run.bin";


#ifdef CMS_BIN_DATA
    constexpr auto GAME_DATA_PATH = GAME_DATA_PATH_CMS;
    constexpr auto GAME_DATA_PATH_FALLBACK = GAME_DATA_PATH_LOCAL;
#else
    constexpr auto GAME_DATA_PATH = GAME_DATA_PATH_LOCAL;
    constexpr auto GAME_DATA_PATH_FALLBACK = GAME_DATA_PATH_CMS;
#endif


namespace em_load
{
    using FetchAttr = emscripten_fetch_attr_t;
    using FetchResponse = emscripten_fetch_t;
    
    
    class FetchContext
    {
    public:
        static constexpr u32 count = 2;

        char url[256];
        char url_fallback[256];

        StateData* data;


        static FetchContext* create(StateData* data)
        {
            auto ctx = mem::alloc<FetchContext>(1, "fetch");

            auto ts = (u32)dt::current_timestamp_i64();

            stb::qsnprintf(ctx->url, 256, "%s?%u", GAME_DATA_PATH, ts);
            stb::qsnprintf(ctx->url_fallback, 256, "%s?%u", GAME_DATA_PATH_FALLBACK, ts);

            ctx->data = data;

            return ctx;
        }


        static void destroy(FetchContext* ctx) { mem::free(ctx); }
    };   


    static ByteView make_byte_view(FetchResponse* res)
    {
        ByteView bytes;
        bytes.data = (u8*)res->data;
        bytes.length = res->numBytes;

        return bytes;
    }


    static void process_asset_data_fail(FetchContext* ctx)
    {
        auto& data = *(ctx->data);

        FetchContext::destroy(ctx);

        data.asset_data.bin_file_path = 0;
        data.asset_data.status = AssetStatus::FailLoad;
    }


    static void process_asset_data_success(FetchContext* ctx, ByteView const& bytes, cstr url)
    {
        auto& data = *(ctx->data);
        auto& asset_data = data.asset_data;

        FetchContext::destroy(ctx);

        auto& buffer = asset_data.bytes;
        if (!mb::create_buffer(buffer, bytes.length, fs::get_file_name(url)))
        {
            asset_data.status = AssetStatus::FailRead;
            return;
        }

        span::copy(bytes, span::make_view(buffer));

        bool ok = true;
        ok &= check_asset_version(data.asset_data);
        if (!ok)
        {
            app_crash("*** Bad asset data version ***");
            data.asset_data.status = AssetStatus::FailRead;
            return;
        }

        ok &= test_game_assets(asset_data);
        if (!ok)
        {
            app_crash("*** Asset tests failed ***");
            asset_data.status = AssetStatus::FailRead;
            return;
        }
        
        read_game_assets(data);
    }


    static void fetch_asset_data_fallback_fail(FetchResponse* res)
    {
        auto ctx = (FetchContext*)(res->userData);
        auto url = ctx->url_fallback;

        app_log("fetch_asset_data_fallback_fail(): %s\n", url);

        process_asset_data_fail(ctx);
        emscripten_fetch_close(res);
    }


    static void fetch_asset_data_fallback_success(FetchResponse* res)
    {
        auto ctx = (FetchContext*)(res->userData);
        auto status = res->status;
        auto url = ctx->url_fallback;

        app_log("fetch_asset_data_fallback_success(): %u/%s\n", status, url);

        if (status == 200)
        {
            auto bytes = make_byte_view(res);
            process_asset_data_success(ctx, bytes, url);
        }
        else
        {
            process_asset_data_fail(ctx);
        }
        
        emscripten_fetch_close(res);
    }
    
    
    static void fetch_asset_data_fallback(FetchContext* ctx)
    {  
        auto url = ctx->url_fallback; 

        FetchAttr attr;
        emscripten_fetch_attr_init(&attr);
        stb::qsnprintf(attr.requestMethod, 4, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.userData = (void*)ctx;
        attr.onsuccess = fetch_asset_data_fallback_success;
        attr.onerror = fetch_asset_data_fallback_fail;

        emscripten_fetch(&attr, url);
    }
    
    
    static void fetch_asset_data_fail(FetchResponse* res)
    {
        auto ctx = (FetchContext*)(res->userData);
        auto url = ctx->url;

        app_log("fetch_asset_data_fail(): %s\n", url);

        emscripten_fetch_close(res);

        // try next url
        fetch_asset_data_fallback(ctx);
    }


    static void fetch_asset_data_success(FetchResponse* res)
    {
        auto ctx = (FetchContext*)(res->userData);
        auto status = res->status;
        auto url = ctx->url;

        app_log("fetch_asset_data_success(): %u/%s\n", status, url);

        if (status == 200)
        {
            auto bytes = make_byte_view(res);
            process_asset_data_success(ctx, bytes, url);
        }
        else
        {
            fetch_asset_data_fallback(ctx);
        }
        
        emscripten_fetch_close(res);        
    }


    static void fetch_asset_data_async(FetchContext* ctx)
    {  
        app_log("fetch_asset_data_async()\n");

        auto url = ctx->url;

        FetchAttr attr;
        emscripten_fetch_attr_init(&attr);
        stb::qsnprintf(attr.requestMethod, 4, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.userData = (void*)ctx;
        attr.onsuccess = fetch_asset_data_success;
        attr.onerror = fetch_asset_data_fail;

        emscripten_fetch(&attr, url);
    }
}


static void load_game_assets(StateData& data)
{   
    data.asset_data.status = AssetStatus::Loading;
    //em_load::fetch_bin_data_async(GAME_DATA_PATH, data);

    auto ctx = em_load::FetchContext::create(&data);
    em_load::fetch_asset_data_async(ctx);
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
            app_crash("*** Error loading asset data ***");
            data.asset_data.status = AssetStatus::FailLoad;
            return;
        }

        ok &= check_asset_version(data.asset_data);
        if (!ok)
        {
            app_crash("*** Bad asset data version ***");
            data.asset_data.status = AssetStatus::FailRead;
            return;
        }

        ok &= test_game_assets(data.asset_data);
        if (!ok)
        {
            app_crash("*** Asset tests failed ***");
            data.asset_data.status = AssetStatus::FailRead;
            return;
        }

        read_game_assets(data);
    }

#endif
}
}