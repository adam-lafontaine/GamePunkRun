namespace game_punk
{
namespace gm_gameplay
{

/* init */

namespace internal
{
    static void init_punk_sprite(StateData& data)
    {
        constexpr auto tile_h = cxpr::TILE_WIDTH;

        auto pos = data.scene.game_position.pos_game();
        pos.x += PLAYER_SCENE_OFFSET;
        pos.y = tile_h;

        auto punk = SpriteDef(data.game_tick, pos, data.bitmaps.push());
        punk.velocity = { 2, 0 };

        u32 bmp_tick = 10 / punk.velocity.x;
        set_animation_spritesheet(punk.animation, data.spritesheet.punk_run, bmp_tick);        

        data.punk_sprite = spawn_sprite(data.sprites, punk);
    }


    static void init_tiles(StateData& data)
    {
        constexpr auto tile_w = cxpr::TILE_WIDTH;

        auto& src = data.tile_state;
        auto& bitmaps = data.tile_bitmaps;
        
        bitmaps.data[0] = data.bitmaps.push_item(to_image_view(src.floor_a));
        bitmaps.data[1] = data.bitmaps.push_item(to_image_view(src.floor_b));

        Vec2Di64 pos = { 0, 0 };
        for (u32 i = 0; i < 20; i++)
        {
            auto tile = TileDef(data.game_tick, pos, data.tile_bitmaps.front());
            spawn_tile(data.tiles, tile);
            pos.x += tile_w;
            data.tile_bitmaps.next();
        }

        data.next_tile_position = GamePosition(pos, DimCtx::Game);
    }
}


/* update */

namespace internal
{
    static void update_game_camera(StateData& data, InputCommand const& cmd)
    {
        if (cmd.camera.move)
        {
            auto dx = ((i32)cmd.camera.east - (i32)cmd.camera.west) * data.camera.speed_px;
            auto dy = ((i32)cmd.camera.north - (i32)cmd.camera.south) * data.camera.speed_px;

            Vec2Di8 delta_px;
            delta_px.x = (i8)dx;
            delta_px.y = (i8)dy;

            move_camera(data.camera, delta_px);
        }
    }


    static void update_tiles(StateData& data)
    {
        constexpr auto tile_w = cxpr::TILE_WIDTH;
        
        auto pos = data.scene.game_position.game.x;
        if (pos % tile_w == 0)
        {
            auto tile = TileDef(data.game_tick, data.next_tile_position.pos_game(), data.tile_bitmaps.front());
            spawn_tile(data.tiles, tile);

            data.next_tile_position.game.x += tile_w;
            data.tile_bitmaps.next();
        }
    }


    static void animate_sprites(StateData& data)
    {
        auto& table = data.sprites;

        auto N = table.capacity;

        auto beg = table.tick_begin;
        auto amn = table.animation;
        auto bmp = table.bitmap_id;

        for (u32 i = 0; i < N; i++)
        {
            if (!is_spawned(table, i))
            {
                continue;
            }

            auto time = data.game_tick - beg[i];
            auto view = get_animation_bitmap(amn[i], time);
            data.bitmaps.item_at(bmp[i]) = to_image_view(view);
        }
    }
}


/* draw */

namespace internal
{
    static void draw_background(StateData& data)
    {
        auto& bg = data.background;
        auto& dq = data.drawq;
        auto& camera = data.camera;
        auto& rng = data.rng;

        auto pos = data.scene.game_position.game.x;

        auto sky = get_sky_animation(bg.sky, data.game_tick);
        auto bg1 = get_animation_pair(bg.bg_1, rng, pos);
        auto bg2 = get_animation_pair(bg.bg_2, rng, pos);
        
        push_draw(dq, sky, camera);
        push_draw(dq, bg1, camera);
        push_draw(dq, bg2, camera);        
    }


    static void draw_tiles(StateData& data)
    {
        constexpr i32 xmin = -(cxpr::GAME_BACKGROUND_WIDTH_PX / 4);
        constexpr i32 ymin = -(cxpr::GAME_BACKGROUND_HEIGHT_PX / 4);

        auto& dq = data.drawq;
        auto& camera = data.camera;
        auto& table = data.tiles;

        auto N = table.capacity;
        
        auto pos = table.position;
        auto bmp = table.bitmap_id;

        for (u32 i = 0; i < N; i++)
        {
            if (!is_spawned(table, i))
            {
                continue;
            }

            auto dps = delta_pos_scene(GamePosition(pos[i], DimCtx::Game), data.scene);
            auto gpos = dps.pos_game();

            if (gpos.x < xmin || gpos.y < ymin)
            {
                despawn_tile(table, i);
                continue;
            }

            auto view = data.bitmaps.item_at(bmp[i]);
            push_draw(dq, view, dps, camera);
        }
    }
    
    
    static void draw_sprites(StateData& data)
    {
        constexpr i32 xmin = -cxpr::GAME_BACKGROUND_WIDTH_PX;
        constexpr i32 ymin = -cxpr::GAME_BACKGROUND_HEIGHT_PX;

        auto& dq = data.drawq;
        auto& camera = data.camera;
        auto& sprites = data.sprites;

        auto tick = data.game_tick;
        auto N = sprites.capacity;

        auto beg = sprites.tick_begin;
        auto end = sprites.tick_end;
        auto pos = sprites.position;
        auto bmp = sprites.bitmap_id;

        for (u32 i = 0; i < N; i++)
        {
            if (tick >= end[i] || beg[i] > end[i])
            {
                continue;
            }

            auto dps = delta_pos_scene(GamePosition(pos[i], DimCtx::Game), data.scene);
            auto gpos = dps.pos_game();

            if (gpos.x < xmin || gpos.y < ymin)
            {
                beg[i] = GameTick64::none();
                sprites.first_id = math::min(i, sprites.first_id);
                continue;
            }
            
            auto view = data.bitmaps.item_at(bmp[i]);
            push_draw(dq, view, dps, camera);
        }
    }
}

}
}


namespace game_punk
{
namespace gm_gameplay
{
    static void init(StateData& data)
    {
        internal::init_punk_sprite(data);
        internal::init_tiles(data);
        
        app_assert(data.punk_sprite.value_ == PLAYER_ID.value_ && "*** Player not first sprite ***");
        
        data.game_tick = GameTick64::zero();
    }
    
    
    static void update(StateData& data, InputCommand const& cmd)
    {
        internal::update_game_camera(data, cmd);

        move_sprites(data.sprites);

        auto& scene_pos = data.scene.game_position.game;
        auto player_pos = data.sprites.position_at(data.punk_sprite);

        scene_pos.x = player_pos.x - PLAYER_SCENE_OFFSET;
        
        internal::update_tiles(data);
        internal::animate_sprites(data);

        internal::draw_background(data);
        internal::draw_tiles(data);
        internal::draw_sprites(data);
    }
}
}