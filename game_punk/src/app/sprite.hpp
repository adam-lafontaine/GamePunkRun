#pragma once


/* spritesheet state */

namespace game_punk
{
    class SpritesheetList
    {
    public:

        SpritesheetView punk_run;
        SpritesheetView punk_idle;
        SpritesheetView punk_jump;
    };


    static void count_spritesheet_list(SpritesheetList& ss_state, MemoryCounts& counts)
    {
        using Punk = bt::Spriteset_Punk;

        constexpr Punk list;
        constexpr auto run = Punk::Items::Punk_run;
        constexpr auto idle = Punk::Items::Punk_idle;
        constexpr auto jump = Punk::Items::Punk_jump;

        count_view(ss_state.punk_run, counts, bt::item_at(list, run));
        count_view(ss_state.punk_idle, counts, bt::item_at(list, idle));
        count_view(ss_state.punk_jump, counts, bt::item_at(list, jump));
    }


    static bool create_spritesheet_list(SpritesheetList& ss_state, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(ss_state.punk_run, memory);
        ok &= create_view(ss_state.punk_idle, memory);
        ok &= create_view(ss_state.punk_jump, memory);

        return ok;
    }
}


/* sprite modes */

namespace game_punk
{
    enum class SpriteMode : u8
    {
        Idle = 0,
        Run,
        Jump,

        Count
    };


    enum class SpriteName : u8
    {
        Punk = 0,

        Count
    };
}


/* animation base */

namespace game_punk
{
    class AnimationBase
    {
    public:
        ContextDims bitmap_dims;
        p32* spritesheet_data = 0;


        SpriteView bitmap_at(u32 id) const
        { 
            p32* data = 0;

            SpriteView view;
            view.dims = bitmap_dims;

            auto dims = bitmap_dims.proc;
            auto offset = id * dims.width * dims.height;
        
            data = spritesheet_data + offset;
            app_assert(data);
        
            view.data = data;

            return view;
        }
    };

}


/* punk sprite animations */

namespace game_punk
{
    class PunkRunAnimation
    {
    public:
        static constexpr u32 bitmap_count = 6;

        AnimationBase base;

        SpriteView get_bitmap(VecSpeed vel, TickQty32 time) const
        {
            auto s = to_delta_px(vel.x);
            auto x = (u32)math::cxpr::clamp(s, 1, 10);
            auto ticks = 10u / x;

            auto t = time.value_ % (bitmap_count * ticks);
            auto bitmap_id = t / ticks;

            return base.bitmap_at(bitmap_id);
        }
    };


    class PunkIdleAnimation
    {
    public:
        static constexpr u32 bitmap_count = 4;

        AnimationBase base;

        SpriteView get_bitmap(TickQty32 time) const
        {
            u32 ticks = 15;

            auto t = time.value_ % (bitmap_count * ticks);
            auto bitmap_id = t / ticks;

            return base.bitmap_at(bitmap_id);
        }
    };


    class PunkJumpAnimation
    {
    public:
        static constexpr u32 bitmap_count = 4;

        AnimationBase base;

        SpriteView get_bitmap(VecSpeed vel, TickQty32 time) const
        {
            u32 bitmap_id = 0;

            auto t = time.value_ % bitmap_count;

            /*switch (vel.y)
            {
            case 0:
                break;
            }*/

            return base.bitmap_at(bitmap_id);
        }
    };

}


/* animation list */

namespace game_punk
{
    static bool init_animation(auto& an, SpritesheetView const& ss)
    {
        bool ok = ss.bitmap_count == an.bitmap_count;
        app_assert(ok && "*** Wrong spritesheet bitmap count ***");

        ok &= ss.data != 0;
        app_assert(ok && "*** Spritesheet data not set ***");

        auto& dims = ss.dims.game;

        ok &= dims.width > dims.height;
        ok &= dims.width % dims.height == 0;
        app_assert(ok && "*** Invalid spritesheet dimensions ***");
        
        an.base.bitmap_dims = ss.bitmap_dims;
        an.base.spritesheet_data = ss.data;        

        return ok;
    }


    class AnimationList
    {
    public:
        PunkRunAnimation punk_run;
        PunkIdleAnimation punk_idle;
        PunkJumpAnimation punk_jump;
        
    };


    static bool init_animation_list(AnimationList& list, SpritesheetList const& spritesheets)
    {
        bool ok = true;

        ok &= init_animation(list.punk_run, spritesheets.punk_run);
        ok &= init_animation(list.punk_idle, spritesheets.punk_idle);
        ok &= init_animation(list.punk_jump, spritesheets.punk_jump);

        return ok;
    }


    using AnimateFn = SpriteView (*)(AnimationList const& list, VecSpeed vel, TickQty32 time);


    static SpriteView animate_error(AnimationList const& list, VecSpeed vel, TickQty32 time)
    {
        auto& an = list.punk_jump.base;
        auto bitmap_id = 0;

        return an.bitmap_at(bitmap_id);
    }


    static SpriteView animate_punk_run(AnimationList const& list, VecSpeed vel, TickQty32 time)
    {
        return list.punk_run.get_bitmap(vel, time);
    }


    static SpriteView animate_punk_idle(AnimationList const& list, VecSpeed vel, TickQty32 time)
    {
        return list.punk_idle.get_bitmap(time);
    }


    static SpriteView animate_punk_jump(AnimationList const& list, VecSpeed vel, TickQty32 time)
    {
        return list.punk_jump.get_bitmap(vel, time);
    }


    static AnimateFn get_punk_animate_fn(SpriteMode mode)
    {
        using Mode = SpriteMode;

        switch (mode)
        {
        case Mode::Idle: return animate_punk_idle;
        case Mode::Run: return animate_punk_run;
        case Mode::Jump: return animate_punk_jump;

        default: return animate_error;
        }
    }


    static AnimateFn get_animate_fn(SpriteName sprite, SpriteMode mode)
    {
        using Sprite = SpriteName;
        using Mode = SpriteMode;

        switch (sprite)
        {
        case Sprite::Punk: return get_punk_animate_fn(mode);

        default: return animate_error;
        }
    }
}


/* sprite table */

namespace game_punk
{    
    
    class SpriteTable
    {
    public:
        struct ID { u32 value_ = 0; };

        u32 capacity = 0;
        u32 first_id = 0;

        GameTick64* tick_begin = 0;
        GameTick64* tick_end = 0;

        VecAcc* acceleration = 0;
        VecSpeed* velocity = 0;
        VecTile* position = 0;
        
        AnimateFn* animate = 0;
        BitmapID* bitmap_id = 0;
        
        GameTick64& tick_begin_at(ID id) { return tick_begin[id.value_]; }
        VecSpeed& velocity_at(ID id) { return velocity[id.value_]; }
        VecTile& position_at(ID id) { return position[id.value_]; }
        AnimateFn& animate_at(ID id) { return animate[id.value_]; }
    };


    using SpriteID = SpriteTable::ID;

    SpriteID& operator ++ (SpriteID& id) { ++id.value_; return id; }
    bool operator < (SpriteID lhs, SpriteID rhs) { return lhs.value_ < rhs.value_; }
    bool operator == (SpriteID lhs, SpriteID rhs) { return lhs.value_ == rhs.value_; }


    class SpriteDef
    {
    public:
        GameTick64 tick_begin = GameTick64::none();
        GameTick64 tick_end = GameTick64::forever();

        VecSpeed velocity = vec_zero<TileSpeed>();
        VecTile position = vec_zero<TileDim>();

        BitmapID bitmap_id;
        AnimateFn animate;


        SpriteDef() = delete;

        SpriteDef(GameTick64 begin, VecTile pos, BitmapID bmp, SpriteName name, SpriteMode mode)
        {
            tick_begin = begin;
            position = pos;
            bitmap_id = bmp;
            animate = get_animate_fn(name, mode);
        }
    };


    static void reset_sprite_table(SpriteTable& table)
    {
        table.first_id = 0;

        span::fill(span::make_view(table.tick_begin, table.capacity), GameTick64::none());
    }


    static void count_table(SpriteTable& table, MemoryCounts& counts, u32 capacity)
    {
        table.capacity = capacity;
        
        add_count<GameTick64>(counts, 2 * capacity);
        add_count<VecAcc>(counts, capacity);
        add_count<VecSpeed>(counts, capacity);
        add_count<VecTile>(counts, capacity);
        add_count<BitmapID>(counts, capacity);
        add_count<AnimateFn>(counts, capacity);
    }


    static bool create_table(SpriteTable& table, Memory& memory)
    {
        if (!table.capacity)
        {
            app_crash("*** SpriteTable not initialized ***");
            return false;
        }

        auto n = table.capacity;

        bool ok = true;

        auto tick_begin = push_mem<GameTick64>(memory, n);
        ok &= tick_begin.ok;

        auto tick_end = push_mem<GameTick64>(memory, n);
        ok &= tick_end.ok;

        auto acc = push_mem<VecAcc>(memory, n);
        ok &= acc.ok;

        auto velocity = push_mem<VecSpeed>(memory, n);
        ok &= velocity.ok;

        auto position = push_mem<VecTile>(memory, n);
        ok &= position.ok;

        auto bmp = push_mem<BitmapID>(memory, n);
        ok &= bmp.ok;

        auto animate = push_mem<AnimateFn>(memory, n);
        ok &= animate.ok;

        if (ok)
        {
            table.tick_begin = tick_begin.data;
            table.tick_end = tick_end.data;
            table.acceleration = acc.data;
            table.velocity = velocity.data;
            table.position = position.data;
            table.bitmap_id = bmp.data;
            table.animate = animate.data;
        }

        return ok;
    }
    

    static void despawn_sprite(SpriteTable& table, u32 i)
    {
        table.tick_begin[i] = GameTick64::none();
        table.first_id = math::min(i, table.first_id);
    }
    
    
    static void despawn_sprite(SpriteTable& table, SpriteID id)
    {
        despawn_sprite(table, id.value_);
    }


    static bool is_spawned(SpriteTable const& table, u32 i)
    {
        return table.tick_begin[i] < table.tick_end[i];
    }    
    
    
    static SpriteID spawn_sprite(SpriteTable& table, SpriteDef const& def)
    {
        auto N = table.capacity;
        auto beg = table.tick_begin;
        auto end = table.tick_end;

        SpriteID id;

        u32 i = table.first_id;
        for (; is_spawned(table, i) && i < N; i++)
        { }

        if (i == N)
        {
            i = 0;
        }

        id.value_ = i;
        table.first_id = i;
        table.tick_begin[i] = def.tick_begin;
        table.tick_end[i] = def.tick_end;
        table.acceleration[i] = vec_zero<TileAcc>();
        table.velocity[i] = def.velocity;
        table.position[i] = def.position;
        table.animate[i] = def.animate;
        table.bitmap_id[i] = def.bitmap_id;

        return id;
    }


    static void move_sprites(SpriteTable const& table)
    {
        auto N = table.capacity;

        auto acc = table.acceleration;
        auto vel = table.velocity;
        auto pos = table.position;

        for (u32 i = 0; i < N; i++)
        {
            vel[i].x += acc[i].x;
            vel[i].y += acc[i].y;

            pos[i].x += vel[i].x;
            pos[i].y += vel[i].y;
        }
    }


    
}
