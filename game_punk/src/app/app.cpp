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
    //constexpr p32 COLOR_BG_1 = img::to_pixel(139, 171, 191); // 8babbf
    //constexpr p32 COLOR_BG_2 = img::to_pixel(86, 106, 137); // 566a89
}


#include "memory.hpp"
#include "app_types.hpp"


/* state */

namespace game_punk
{
    enum class GameMode : int
    {
        Error,
        Loading,
        Title,
    };


    using FnUpdate = void (*)(StateData& data, InputCommand const& cmd);



    class StateData
    {
    public:

        static constexpr u32 game_width = cxpr::GAME_BACKGROUND_WIDTH_PX;
        static constexpr u32 game_height = cxpr::GAME_BACKGROUND_HEIGHT_PX;

        GameMode game_mode;
        FnUpdate fn_update;

        BackgroundState background;
        SpritesheetState spritesheet;
        TileState tiles;
        UIState ui;

        ScreenCamera camera;

        DrawQueue drawq;

        SpriteAnimation punk_animation;

        Memory memory;

        AssetData asset_data;
        LoadAssetQueue loadq;

        GameTick64 game_tick;

        u8 camera_speed_px;

        Randomf32 rng;
    };


    static void reset_state_data(StateData& data)
    {
        data.game_mode = GameMode::Title;

        data.game_tick = GameTick64::zero();
        data.camera_speed_px = 2;

        reset_background_state(data.background);
        reset_screen_camera(data.camera);
        reset_random(data.rng);

        reset_ui_state(data.ui);
        set_ui_color(data.ui, 20);
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

        destroy_asset_data(data.asset_data);
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
        count_draw(data.drawq, counts, 50);
        
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
        ok &= create_draw(data.drawq, data.memory);

        ok &= verify_allocated(data.memory);

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

        auto ok = create_state_data_memory(data);
        if (!ok)
        {
            destroy_state_data(state);
            return AppError::Memory;
        }

        return AppError::None;
    }
}

#include "assets.hpp"


/* update */

namespace game_punk
{
    static void begin_update(StateData& data)
    {
        ++data.game_tick;
        begin_ui_frame(data.ui);
        reset_draw(data.drawq); 
    }


    static void end_update(StateData& data)
    {
        refresh_random(data.rng);

        push_load_background(data.background.bg_1, data.loadq);
        push_load_background(data.background.bg_2, data.loadq);

        load_all(data.asset_data, data.loadq);
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
        auto& dq = data.drawq;
        auto& camera = data.camera;
        auto& rng = data.rng;

        auto sky = get_sky_animation(bg.sky, data.game_tick);
        auto bg1 = get_animation_pair(bg.bg_1, rng, data.game_tick.value_);
        auto bg2 = get_animation_pair(bg.bg_2, rng, data.game_tick.value_);
        
        push_draw(dq, sky, camera);
        push_draw(dq, bg1, camera);
        push_draw(dq, bg2, camera);        
    }
    
    
    static void draw_tiles(StateData& data)
    {
        auto& dq = data.drawq;
        auto& camera = data.camera;

        auto pos = BackgroundPosition(0, 0, DimCtx::Game);
        auto& gpos = pos.game;

        // draw floor tiles
        TileView tiles[2] = { data.tiles.floor_a, data.tiles.floor_b };
        auto tile_w = tiles[0].dims.game.width;
        
        gpos.x = 0;
        gpos.y = 0;

        u32 tile_id = 0;
        for (; pos.game.x < data.game_width; pos.game.x += tile_w)
        {
            push_draw(dq, tiles[tile_id], pos, camera);
            tile_id = !tile_id;
        }
    }


    static void draw_sprites(StateData& data)
    {
        constexpr auto tile_h = bt::Tileset_ex_zone().items[0].height;

        auto& dq = data.drawq;
        auto& camera = data.camera;

        // draw sprite
        auto frame = get_animation_bitmap(data.punk_animation, data.game_tick.value_);
        auto camera_w = CAMERA_DIMS.game.width;
        auto sprite_w = frame.dims.game.width;

        auto x = 16 + (i32)(camera_w - sprite_w) / 2;        
        auto y = (i32)tile_h;

        auto pos = BackgroundPosition(x, y, DimCtx::Game);

        push_draw(dq, frame, pos, camera);
    }


    static void render_screen(StateData& data)
    {
        auto s = to_span(data.camera);
        span::fill(s, COLOR_TRANSPARENT);

        draw(data.drawq);

        for (u32 i = 0; i < s.length; i++)
        {
            s.data[i].alpha = 255;
        }
    }
}


/* game modes */

namespace game_punk
{
    
    static void set_game_mode(StateData& data, GameMode mode)
    {
        switch (mode)
        {
        case GameMode::Error:
            break;

        case GameMode::Loading:
            app_log("Loading\n");
            assets::load_game_assets(data);
            break;

        case GameMode::Title:
            app_log("Title\n");
            set_animation_spritesheet(data.punk_animation, data.spritesheet.punk_run);
            data.game_tick = GameTick64::zero();
            break;
        }

        data.game_mode = mode;
    }


    static void update_loading(StateData& data)
    {        
        switch (data.asset_data.status)
        {
        case AssetStatus::None:
        case AssetStatus::FailLoad:
        case AssetStatus::FailRead:
            set_game_mode(data, GameMode::Error);
            break;

        case AssetStatus::Success:
            set_game_mode(data, GameMode::Title);
            break;

        case AssetStatus::Loading: return;
        }
    }
    
    
    static void update_title(StateData& data, InputCommand const& cmd)
    {
        update_game_camera(data, cmd);
        update_text_color(data, cmd);

        draw_background(data);
        draw_tiles(data);
        draw_sprites(data);
    }


    static void game_mode_update(StateData& data, InputCommand const& cmd)
    {
        using GM = GameMode;

        switch (data.game_mode)
        {
        case GM::Error:
            app_crash("GameMode::Error\n");
            break;

        case GM::Loading:
            update_loading(data);   
            break;

        case GM::Title:
            update_title(data, cmd);
            break;
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
        auto dims = CAMERA_DIMS.proc;
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
        auto dims = CAMERA_DIMS.proc;
        
        auto w_max = !available_dims.x ? dims.width : available_dims.x;
        auto h_max = !available_dims.y ? dims.height : available_dims.y;

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

        if (result.success)
        {
            result.app_dimensions = { 
                dims.width,
                dims.height
            };
        }

        return result;
    }


    bool set_screen_memory(AppState& state, image::ImageView screen)
    {
        state.screen = screen;

        auto& data = get_data(state);
        auto& camera = data.camera;
        auto& bg = data.background;
        auto& sprite = data.spritesheet;

        bool ok = true;

        ok &= init_screen_camera(camera, screen);

        reset_state_data(data);

        app_assert(ok && "*** Error set_screen_memory ***");

        set_game_mode(data, GameMode::Loading);

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
        
        begin_update(data);

        auto cmd = map_input(input);

        game_mode_update(data, cmd);

        render_screen(data);

        end_update(data);

        app_crash("*** Update not implemented ***");
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
        
        begin_update(data);

        auto cmd = map_input(input);

        game_mode_update(data, cmd);

        render_screen(data);

        end_update(data);
    }

#endif
}

