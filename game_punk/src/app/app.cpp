#include "app.hpp"
#include "../../../libs/math/math.hpp"


#ifndef app_assert
#include <cassert>
#define app_assert(condition) assert(condition)
#endif

#ifndef app_log
#include <cstdio>
#define app_log(...) printf(__VA_ARGS__)
#endif

#ifndef app_crash
#define app_crash(message) assert(false && message)
#endif


/* definitions */

namespace game_punk
{
    namespace img = image;
    namespace mb = memory_buffer;

    using p32 = img::Pixel;
    using Image = img::Image;
    using ImageGray = img::ImageGray;
    using ImageView = img::ImageView;
    using SubView = img::SubView;
    using ImageMask = img::ImageGray;
    using Buffer8 = MemoryBuffer<u8>;
    using Buffer16 = MemoryBuffer<u16>;
    using Buffer32 = MemoryBuffer<u32>;
    using Buffer64 = MemoryBuffer<u64>;
    using Span32 = SpanView<p32>;
    using Span8 = SpanView<u8>;
    using Input = input::Input;


    constexpr u32 WIDTH_4K = 3840;
    constexpr u32 HEIGHT_4K = 2160;

    constexpr u32 WIDTH_HD = WIDTH_4K / 2;
    constexpr u32 HEIGHT_HD = HEIGHT_4K / 2;

    

    constexpr p32 COLOR_TRANSPARENT = img::to_pixel(0, 0, 0, 0);
    constexpr p32 COLOR_BLACK = img::to_pixel(0);
    constexpr p32 COLOR_WHITE = img::to_pixel(255);
    //constexpr p32 COLOR_BG_1 = img::to_pixel(139, 171, 191);
    //constexpr p32 COLOR_BG_2 = img::to_pixel(86, 106, 137);


    enum class RenderLayerId : u32
    {
        Sprites = 0,
        Background_2,
        Background_1,
        Sky,
        Count
    };


    enum class CameraLayerId : u32
    {
        UI = 0,
        HUD,
        Count
    };
}


#include "memory.hpp"
#include "app_types.hpp"
#include "assets.hpp"


/* constants */

namespace game_punk
{
    
}


/* helpers */

namespace game_punk
{
    
}


/* state */

namespace game_punk
{
    class StateData
    {
    public:

        static constexpr u32 game_width = cxpr::GAME_WIDTH_PX;
        static constexpr u32 game_height = cxpr::GAME_HEIGHT_PX;

        BackgroundState background;
        SpritesheetState spritesheet;
        TileState tiles;
        UIState ui;

        GameCamera camera;

        RenderState render;       

        DrawQueue drawq;
        AnimationFast punk_animation;

        Memory memory;
        AssetData asset_data;

        GameTick64 game_tick;

        u8 camera_speed_px;


        // Temp icon
        CtxPt2Di32 icon_pos;
    };


    static void reset_state_data(StateData& data)
    {
        data.game_tick = GameTick64::zero();
        data.camera_speed_px = 2;

        reset_background_state(data.background);
        reset_render_state(data.render);
        reset_game_camera(data.camera);

        set_ui_color(data.ui, 16);

        data.icon_pos.game.x = 86;
        data.icon_pos.game.y = 59;
    }


    static inline StateData& get_data(AppState const& state)
    {
        return *state.data_;
    }


    static void destroy_state_data(AppState& state)
    {
        if (!state.data_)
        {
            return;
        }

        auto& data = get_data(state);

        destroy_memory(data.memory);

        mem::free(state.data_);
        state.data_ = 0;
    }


    static bool create_state_data_memory(StateData& data)
    {
        MemoryCounts counts;

        Vec2Du32 game_dims = {
            data.game_width,
            data.game_height
        };        

        count_background_state(data.background, counts);
        count_spritesheet_state(data.spritesheet, counts);
        count_tile_state(data.tiles, counts);
        count_ui_state(data.ui, counts);
        
        data.memory = create_memory(counts);
        if (!data.memory.ok)
        {
            return false;
        }

        bool ok = true;

        ok &= create_background_state(data.background, data.memory);
        ok &= create_spritesheet_state(data.spritesheet, data.memory);
        ok &= create_tile_state(data.tiles, data.memory);
        ok &= create_ui_state(data.ui, data.memory);

        return ok;
    }
    
    
    static AppError create_state_data(AppState& state)
    {
        auto data_p = mem::alloc<StateData>(1, "StateData");
        if (!data_p)
        {
            return AppError::Memory;
        }

        state.data_ = data_p;

        auto& data = get_data(state);

        if (!assets::load_asset_data(data.asset_data))
        {
            return AppError::Assets;
        }

        init_render_state(data.render);

        auto ok = create_state_data_memory(data);
        if (!ok)
        {
            destroy_state_data(state);
            return AppError::Memory;
        }
        
        ok &= assets::load_background_assets(data.asset_data, data.background);
        ok &= assets::load_spritesheet_assets(data.asset_data, data.spritesheet);
        ok &= assets::load_tile_assets(data.asset_data, data.tiles);
        ok &= assets::load_ui_assets(data.asset_data, data.ui);

        destroy_asset_data(data.asset_data);

        if (!ok)
        {
            destroy_state_data(state);
            return AppError::Assets;
        }

        return AppError::None;
    }
}


/* update */

namespace game_punk
{
    static void start_new_frame(StateData& data)
    {
        ++data.game_tick;
        clear_render_layer(data.spritesheet.layer_sprite);
        start_ui_frame(data.ui);
        clear_camera_layer(data.render.screen_out);
        reset_draw(data.drawq); 
    }


    static void update_game_camera(StateData& data, InputCommand const& cmd)
    {
        if (cmd.camera.move)
        {
            auto dx = ((i32)cmd.camera.east - (i32)cmd.camera.west) * data.camera_speed_px;
            auto dy = ((i32)cmd.camera.north - (i32)cmd.camera.south) * data.camera_speed_px;

            Vec2Di8 delta_px;
            delta_px.x = (i8)dx;
            delta_px.y = (i8)dy;

            move_camera(data.camera, delta_px);
        }

        // temp        
        if (cmd.icon.move)
        {            
            auto dx = ((i32)cmd.icon.east - (i32)cmd.icon.west);
            auto dy = ((i32)cmd.icon.north - (i32)cmd.icon.south);            

            data.icon_pos.game.x += dx;
            data.icon_pos.game.y += dy;
        }
    }


    static void update_text_color(StateData& data, InputCommand const& cmd)
    {
        auto N = data.ui.CTS;
        auto id = data.ui.font_color_id;

        if (cmd.text.changed)
        {
            id += (u8)cmd.text.up;
            set_ui_color(data.ui, (id % N));     
        }
    }


    static void draw_background(StateData& data)
    {
        auto& bg = data.background;

        render_background_sky(bg, data.game_tick);

        copy(bg.data.bg_1, bg.layer_bg_1);
        copy(bg.data.bg_2, bg.layer_bg_2);
    }
    
    
    static void draw_tiles(StateData& data)
    {
        auto& layer = data.spritesheet.layer_sprite;
        if (!layer_active(layer))
        {
            return;
        }

        CtxPt2Di32 pos;
        auto& gpos = pos.game;

        // draw floor tiles
        TileView tiles[2] = { data.tiles.floor_a, data.tiles.floor_b };
        auto tile_w = tiles[0].dims.game.width;
        
        gpos.x = 0;
        gpos.y = 0;

        u32 tile_id = 0;
        for (; pos.game.x < data.game_width; pos.game.x += tile_w)
        {
            push_draw_tile(data.drawq, tiles[tile_id], layer, pos);
            tile_id = !tile_id;
        }
    }


    static void draw_sprites(StateData& data)
    {
        constexpr auto tile_h = bt::Tileset_ex_zone().items[0].height;

        auto& layer = data.spritesheet.layer_sprite;
        if (!layer_active(layer))
        {
            return;
        }

        CtxPt2Di32 pos;
        auto& gpos = pos.game;

        // draw sprite
        auto frame = get_animation_bitmap(data.punk_animation, data.game_tick);
        auto camera_w = data.camera.viewport_dims_px.game.width;
        auto sprite_w = frame.dims.game.width;

        gpos.x = 16 + (i32)(camera_w - sprite_w) / 2;        
        gpos.y = (i32)tile_h;

        push_draw_sprite(data.drawq, frame, layer, pos);
    }


    static void draw_ui_title(StateData& data)
    {
        auto src = data.ui.data.title;
        auto dst = to_image_view(data.ui.ui);

        CtxPt2Di32 pos;

        i32 x = 100;
        i32 y = (dst.height - src.height) / 2;

        Point2Di32 p = { x, y };
        push_draw_view(data.drawq, src, dst, p);
    }


    static void draw_ui_icon(StateData& data)
    {
        // TEMP
        auto src = get_ui_icon(data.ui);
        auto dst = data.ui.ui;

        push_draw_ui(data.drawq, src, dst, data.icon_pos);
    }
    
    
    static void draw_ui(StateData& data)
    {
        auto red = img::to_pixel(255, 0, 0);
        auto green = img::to_pixel(0, 255, 0);

        auto& ui = data.ui.ui;
        
        if (layer_active(ui))
        {         
            draw_ui_title(data);
            draw_ui_icon(data);
        }


        auto& hud = data.ui.hud;
        if (layer_active(hud))
        {
            span::fill(to_span(hud), green);
        }
    }


    static void render_screen(StateData& data, ImageView screen)
    {
        draw(data.drawq);

        render_to_screen(data.render, data.camera);

        auto s = img::to_span(screen);
        for (u32 i = 0; i < s.length; i++)
        {
            s.data[i].alpha = 255;
        }
    }
}


/* api */

namespace game_punk
{
    AppResult init(AppState& state)
    {
        AppResult result;
        result.success = false;

        result.error = create_state_data(state);

        if (result.error != AppError::None)
        {
            return result;
        }

        auto& data = get_data(state);        
        
        // reversed
        auto& dims = data.camera.viewport_dims_px.proc;
        result.app_dimensions = { 
            dims.width,
            dims.height
        };

        result.success = result.error == AppError::None;

        return result;
    }


    AppResult init(AppState& state, Vec2Du32 available_dims)
    {
        AppResult result;
        result.success = false;

        result.error = create_state_data(state);

        if (result.error != AppError::None)
        {
            return result;
        }

        auto& data = get_data(state);        
        
        // reversed
        auto& dims = data.camera.viewport_dims_px.proc;
        result.app_dimensions = { 
            dims.width,
            dims.height
        };

        auto w_max = available_dims.x;
        auto h_max = available_dims.y;

        auto bad_w = w_max < dims.width;
        auto bad_h = h_max < dims.height;

        if (bad_w && bad_h)
        {
            result.error = AppError::ScreeenDimensions;
        }
        else if (bad_w)
        {
            result.error = AppError::ScreenWidth;
        }
        else if (bad_h)
        {
            result.error = AppError::ScreenHeight;
        }

        result.success = result.error == AppError::None;

        return result;
    }


    bool set_screen_memory(AppState& state, image::ImageView screen)
    {
        state.screen = screen;

        auto& data = get_data(state);
        auto& bg = data.background;
        auto& sprite = data.spritesheet;
        auto& render = data.render;
        auto& ui = data.ui;

        bool ok = true;
        
        ok &= set_out_layer(render, screen);

        ok &= set_render_layer(render, bg.layer_sky, RenderLayerId::Sky);
        ok &= set_render_layer(render, bg.layer_bg_1, RenderLayerId::Background_1);
        ok &= set_render_layer(render, bg.layer_bg_2, RenderLayerId::Background_2);
        ok &= set_render_layer(render, sprite.layer_sprite, RenderLayerId::Sprites);

        ok &= set_ui_layer(render, ui.ui, CameraLayerId::UI);
        ok &= set_ui_layer(render, ui.hud, CameraLayerId::HUD);

        reset_state_data(data);

        ok &= set_animation_spritesheet(data.punk_animation, sprite.data.punk_run);

        app_assert(ok && "*** Error set_screen_memory ***");        

        return ok;
    }


    void reset(AppState& state)
    {
        auto& data = get_data(state);
        reset_state_data(data);
    }


    void close(AppState& state)
    {
        destroy_state_data(state);
    }


    void update(AppState& state, input::Input const& input)
    {
        auto& data = get_data(state);

        data.background.layer_sky.active.set_active(1);
        data.background.layer_bg_1.active.set_active(1);
        data.background.layer_bg_2.active.set_active(1);
        data.spritesheet.layer_sprite.active.set_active(1);
        data.ui.hud.active.set_active(0);
        data.ui.ui.active.set_active(1);

        start_new_frame(data);

        auto cmd = map_input(input);

        update_game_camera(data, cmd);
        update_text_color(data, cmd);

        draw_background(data);
        draw_tiles(data);
        draw_sprites(data);
        draw_ui(data);

        render_screen(data, state.screen);

        //app_crash("*** Update not implemented ***");
    }


    cstr decode_error(AppError error)
    {
        switch (error)
        {
        case AppError::Memory:            return "Memory Error";
        case AppError::Assets:            return "Game Assets Error";
        case AppError::ScreeenDimensions: return "Screen dimensions too small";
        case AppError::ScreenWidth:       return "Screen width too small";
        case AppError::ScreenHeight:      return "Screen height too small";
        default:                          return "Game OK";
        }
    }
    
}


/* debugging context */

namespace game_punk
{
#ifndef GAME_PUNK_RELEASE


    static void reset(DebugContext& dbg)
    {
        dbg.layers.all = 0xFF;
        dbg.layers.hud = 0;
        //dbg.layers.ui = 0;
    }


    bool set_screen_memory_dbg(AppState& state, image::ImageView screen, DebugContext& dbg)
    {
        reset(dbg);
        auto ok = set_screen_memory(state, screen);



        return ok;
    }


    void reset_dbg(AppState& state, DebugContext& dbg)
    {
        reset(dbg);
        reset(state);
    }


    void update_dbg(AppState& state, input::Input const& input, DebugContext const& dbg)
    {
        auto& data = get_data(state);

        data.background.layer_sky.active.set_active(dbg.layers.sky);
        data.background.layer_bg_1.active.set_active(dbg.layers.bg1);
        data.background.layer_bg_2.active.set_active(dbg.layers.bg2);
        data.spritesheet.layer_sprite.active.set_active(dbg.layers.sprite);
        data.ui.hud.active.set_active(dbg.layers.hud);
        data.ui.ui.active.set_active(dbg.layers.ui);

        start_new_frame(data);

        auto cmd = map_input(input);

        update_game_camera(data, cmd);
        update_text_color(data, cmd);

        draw_background(data);
        draw_tiles(data);
        draw_sprites(data);
        draw_ui(data);

        render_screen(data, state.screen);


    }

#endif
}

