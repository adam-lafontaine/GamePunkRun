namespace game_punk
{
namespace gm_gameplay
{

/* init */

namespace internal
{
    static void init_punk_sprite(StateData& data)
    {
        constexpr auto tile_h = cxpr::TILE_HEIGHT_PX;

        auto& bitmaps = data.bitmaps;
        auto& animations = data.animations;
        auto& player = data.player_state;
        auto& spritesheets = data.spritesheets;

        auto bmp = bitmaps.push();

        auto pos = data.scene.game_position.pos_game();
        pos.x += to_delta_tile(PLAYER_SCENE_OFFSET);
        pos.y += to_delta_tile(tile_h);

        auto mode = SpriteMode::Idle;

        auto punk = SpriteDef(data.game_tick, pos, bmp, SpriteName::Punk, mode);

        player.sprite = spawn_sprite(data.sprites, punk);
        player.current_mode = mode;
    }


    static void init_tiles(StateData& data)
    {
        const auto zero = TileDim::zero();
        const auto one = units::TileDelta::make(1.0f);

        auto& src = data.tile_state;
        auto& bitmaps = data.tile_bitmaps;
        
        bitmaps.data[0] = data.bitmaps.push_item(to_image_view(src.floor_a));
        bitmaps.data[1] = data.bitmaps.push_item(to_image_view(src.floor_b));

        VecTile pos = { zero, zero };
        for (u32 i = 0; i < 20; i++)
        {
            auto tile = TileDef(data.game_tick, pos, data.tile_bitmaps.front());
            spawn_tile(data.tiles, tile);
            pos.x += one;
            data.tile_bitmaps.next();
        }

        data.next_tile_position = TilePosition(pos, DimCtx::Game);
    }
}


/* update */

namespace internal
{
    static void update_game_camera(StateData& data, InputCommand const& cmd)
    {
        if (cmd.camera.move)
        {
            auto dx = ((i32)cmd.camera.right - (i32)cmd.camera.left) * data.camera.speed_px;
            auto dy = ((i32)cmd.camera.up - (i32)cmd.camera.down) * data.camera.speed_px;

            Vec2Di8 delta_px;
            delta_px.x = (i8)dx;
            delta_px.y = (i8)dy;

            move_camera(data.camera, delta_px);
        }
    }


    static void update_player(StateData& data, InputCommand const& cmd)
    {
        auto& player = data.player_state;
        //auto& vel = data.sprites.velocity_px_at(player.sprite);

        auto mode = player.current_mode;
        
        if (cmd.action)
        {
            switch (mode)
            {
            case SpriteMode::Idle:
                mode = SpriteMode::Run;
                break;

            case SpriteMode::Run:
                mode = SpriteMode::Idle;
                break;

            default:
                break;
            }

            set_player_mode(player, data.sprites, mode);
        }
        else if (cmd.jump)
        {
            set_player_mode(player, data.sprites, SpriteMode::Jump);
        }
        /*else
        {
            switch (mode)
            {
            case SpriteMode::Jump:
                vel.y -= 1; // TODO sprite stuff
                vel.y = math::max(vel.y, -1);
                break;

            default:
                break;
            }
        }*/
    }


    static void update_tiles(StateData& data)
    {
        constexpr auto one = units::TileDelta::make(1.0f);
        constexpr auto tile_w = cxpr::TILE_WIDTH_PX;
        constexpr auto limit = (i32)(cxpr::GAME_BACKGROUND_WIDTH_PX - 2 * tile_w);

        auto scene = to_scene_pos(data.next_tile_position, data.scene);
        
        auto delta = scene.pos_game().x.get();

        if (delta < limit)
        {
            auto pos = data.next_tile_position.pos_game();
            auto tile = TileDef(data.game_tick, pos, data.tile_bitmaps.front());
            spawn_tile(data.tiles, tile);

            data.next_tile_position.game.x += one;
            data.tile_bitmaps.next();
        }
    }


    static void animate_sprites(StateData& data)
    {
        auto& table = data.sprites;

        auto N = table.capacity;

        auto beg = table.tick_begin;
        auto afn = table.animate;
        auto bmp = table.bitmap_id;

        for (u32 i = 0; i < N; i++)
        {
            SpriteID id = { i };

            if (!is_spawned(table, id))
            {
                continue;
            }

            auto vel = table.get_tile_velocity(id);

            auto time = data.game_tick - beg[i];
            auto view = afn[i](data.animations, vel, time);
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

        auto tile = data.scene.game_position.pos_game().x;

        auto pos = to_pixel_pos(tile);

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

        auto& scene = data.scene;
        auto& dq = data.drawq;
        auto& camera = data.camera;
        auto& table = data.tiles;

        auto N = table.capacity;
        
        auto pos = table.position;
        auto bmp = table.bitmap_id;

        for (u32 i = 0; i < N; i++)
        {
            TileID id = { i };

            if (!is_spawned(table, id))
            {
                continue;
            }

            auto spos = to_scene_pos(pos[i], scene);
            auto gpos = spos.pos_game();

            if (gpos.x.get() < xmin || gpos.y.get() < ymin)
            {
                despawn_tile(table, id);
                continue;
            }            

            auto view = data.bitmaps.item_at(bmp[i]);
            push_draw(dq, view, spos, camera);
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
        auto bmp = sprites.bitmap_id;

        for (u32 i = 0; i < N; i++)
        {
            if (tick >= end[i] || beg[i] > end[i])
            {
                continue;
            }

            SpriteID id = { i };

            auto tile = sprites.get_tile_pos(id);

            auto spos = to_scene_pos(tile, data.scene);
            auto gpos = spos.pos_game();

            if (gpos.x.get() < xmin || gpos.y.get() < ymin)
            {
                beg[i] = GameTick64::none();
                sprites.first_id = math::min(i, sprites.first_id);
                continue;
            }
            
            auto view = data.bitmaps.item_at(bmp[i]);
            push_draw(dq, view, spos, camera);
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
        
        app_assert(data.player_state.sprite == PLAYER_ID && "*** Player not first sprite ***");
        
        data.game_tick = GameTick64::zero();
    }
    
    
    static void update(StateData& data, InputCommand const& cmd)
    {
        internal::update_game_camera(data, cmd);
        internal::update_player(data, cmd);
        
        move_sprites_xy(data.sprites);

        auto& scene_pos = data.scene.game_position.game;
        auto player_pos = data.sprites.get_tile_x(data.player_state.sprite);

        scene_pos.x = player_pos;
        scene_pos.x -= to_delta_tile(PLAYER_SCENE_OFFSET);
        
        internal::update_tiles(data);
        internal::animate_sprites(data);

        internal::draw_background(data);
        internal::draw_tiles(data);
        internal::draw_sprites(data);
    }
}
}