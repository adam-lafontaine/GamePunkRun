namespace game_punk
{
namespace gm_gameplay
{
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


    static void update_animation_bitmaps(StateData& data)
    {
        auto time = data.game_tick - data.sprites.tick_begin_at(data.punk_sprite);
        auto view = get_animation_bitmap(data.punk_animation, time);
        data.bitmaps.at(data.punk_bitmap) = to_image_view(view);
    }


    static void update_tiles(StateData& data)
    {
        constexpr auto tile_w = bt::Tileset_ex_zone().items[0].width;
        
        auto pos = data.scene.game_position.game.x;
        if (pos % tile_w == 0)
        {
            auto tile = SpriteDef(data.game_tick, data.next_tile_position.pos_game(), data.tile_bitmaps.front());
            spawn_sprite(data.sprites, tile);
            data.next_tile_position.game.x += tile_w;
            data.tile_bitmaps.next();
        }
    }


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
            auto pos = dps.pos_game();

            if (pos.x < xmin || pos.y < ymin)
            {
                beg[i] = GameTick64::none();
                sprites.first_id = math::min(i, sprites.first_id);
                continue;
            }

            auto time = tick - beg[i];
            auto view = data.bitmaps.at(bmp[i]);
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
        set_animation_spritesheet(data.punk_animation, data.spritesheet.punk_run);
        data.game_tick = GameTick64::zero();
    }
    
    
    static void update(StateData& data, InputCommand const& cmd)
    {
        internal::update_game_camera(data, cmd);

        move_sprites(data.sprites);

        auto& scene_pos = data.scene.game_position.game;
        auto player_pos = data.sprites.position_at(data.punk_sprite);

        scene_pos.x = player_pos.x - PLAYER_SCENE_OFFSET;

        internal::update_animation_bitmaps(data);
        internal::update_tiles(data);

        internal::draw_background(data);
        internal::draw_sprites(data);
    }
}
}