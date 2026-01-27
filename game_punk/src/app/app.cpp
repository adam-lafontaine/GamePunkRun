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


#include "units.hpp"
#include "memory.hpp"
#include "app_types.hpp"
#include "image_view.hpp"
#include "sky_background.hpp"
#include "tile.hpp"
#include "sprite.hpp"
#include "draw.hpp"
#include "app_state.hpp"


/* state */

namespace game_punk
{

    constexpr SpriteID PLAYER_ID = {0}; // ?
    constexpr u32 PLAYER_SCENE_OFFSET = cxpr::GAME_CAMERA_WIDTH_PX / 2 + 10;
    
    
    enum class GameMode : int
    {
        Error,
        Title,
        Gameplay
    };


    class StateData
    {
    public:

        static constexpr u32 game_width = cxpr::GAME_BACKGROUND_WIDTH_PX;
        static constexpr u32 game_height = cxpr::GAME_BACKGROUND_HEIGHT_PX;

        GameMode game_mode;

        BackgroundState background;
        SpritesheetList spritesheets;
        TileState tile_state;
        UIState ui;

        BitmapTable bitmaps;
        AnimationList animations;

        GameScene scene;
        SceneCamera camera;

        DrawQueue drawq;        

        TileTable tiles;
        SpriteTable sprites;

        PlayerState player_state;

        TilePosition next_tile_position;
        RingStackBuffer<BitmapID, 2> tile_bitmaps;

        Memory memory;

        AssetData asset_data;
        LoadAssetQueue loadq;

        GameTick64 game_tick;

        Randomf32 rng;
    };


    static void reset_state_data(StateData& data)
    {
        data.game_mode = GameMode::Title;

        data.game_tick = GameTick64::zero();

        reset_background_state(data.background);
        reset_game_scene(data.scene);
        reset_screen_camera(data.camera);
        reset_random(data.rng);

        reset_ui_state(data.ui);
        set_ui_color(data.ui, 20);

        reset_table(data.bitmaps);
        reset_tile_table(data.tiles);
        reset_sprite_table(data.sprites);
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
        count_spritesheet_list(data.spritesheets, counts);
        count_tile_state(data.tile_state, counts);
        count_ui_state(data.ui, counts);
        count_queue(data.drawq, counts, 50);
        count_queue(data.loadq, counts, 10);
        count_random(data.rng, counts);
        count_table(data.tiles, counts, 50);
        count_table(data.sprites, counts, 50);
        count_table(data.bitmaps, counts, 50);
        
        data.memory = create_memory(counts);
        if (!data.memory.ok)
        {
            return false;
        }

        bool ok = true;

        ok &= create_background_state(data.background, data.memory);
        ok &= create_spritesheet_list(data.spritesheets, data.memory);
        ok &= create_tile_state(data.tile_state, data.memory);
        ok &= create_ui_state(data.ui, data.memory);
        ok &= create_queue(data.drawq, data.memory);
        ok &= create_queue(data.loadq, data.memory);
        ok &= create_random(data.rng, data.memory);
        ok &= create_table(data.tiles, data.memory);
        ok &= create_table(data.sprites, data.memory);
        ok &= create_table(data.bitmaps, data.memory);

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


namespace game_punk
{
    static void set_game_mode(StateData& data, GameMode mode);
}


#include "gm_title.hpp"
#include "gm_gameplay.hpp"


/* game modes */

namespace game_punk
{
    
    static void set_game_mode(StateData& data, GameMode mode)
    {
        switch (mode)
        {
        case GameMode::Error:
            app_crash("Error\n");
            break;

        case GameMode::Title:
            app_log("Title\n");
            gm_title::init(data);
            break;

        case GameMode::Gameplay:
            app_log("Gameplay\n");
            gm_gameplay::init(data);
            break;
        }

        data.game_mode = mode;
    }


    static void game_mode_update(StateData& data, InputCommand const& cmd)
    {
        using GM = GameMode;

        switch (data.game_mode)
        {
        case GM::Error:
            app_crash("GameMode::Error\n");
            break;

        case GM::Title:
            gm_title::update(data, cmd);
            break;

        case GM::Gameplay:
            gm_gameplay::update(data, cmd);
            break;
        }
    }
}


/* update */

namespace game_punk
{
    static void begin_update(StateData& data)
    {
        ++data.game_tick;
        reset_draw(data.drawq); 
    }


    static void end_update(StateData& data)
    {
        refresh_random(data.rng);

        

        load_all(data.asset_data, data.loadq);
    }


    static void render_screen(StateData& data)
    {
        draw(data.drawq);
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
            result.error = AppError::ScreenDimensions;
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

        bool ok = true;

        ok &= init_screen_camera(camera, screen);
        ok &= init_animation_list(data.animations, data.spritesheets);

        reset_state_data(data);

        app_assert(ok && "*** Error set_screen_memory ***");

        set_game_mode(data, GameMode::Title);

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
        case AppError::Memory:           return "Memory Error";
        case AppError::Assets:           return "Game Assets Error";
        case AppError::ScreenDimensions: return "Screen dimensions too small";
        case AppError::ScreenWidth:      return "Screen width too small";
        case AppError::ScreenHeight:     return "Screen height too small";
        default:                         return "Game OK";
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

