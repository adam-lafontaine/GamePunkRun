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


/* acceleration helpers */

namespace game_punk
{
    using AccelerateFn = TileAcc (*)(TileSpeed speed, TickQty32 time);


    static TileAcc accelerate_zero(TileSpeed speed, TickQty32 time)
    {
        return TileAcc::zero();
    }


    static TileAcc accelerate_stop(TileSpeed speed)
    {
        auto delta = TileValue::make(-speed.get());

        return TileAcc::make(delta);
    }


    static constexpr TileAcc accelerate_px(f32 px)
    {
        return TileAcc::make(px_to_tile_value(px));
    }


    static constexpr TileSpeed speed_px(f32 px)
    {
        return TileSpeed::make(px_to_tile_value(px));
    }
}


/* punk sprite acceleration */

namespace game_punk
{
    


    static TileAcc accelerate_punk_idle_x(TileSpeed speed, TickQty32 time)
    {
        constexpr auto min_speed = TileValue::min_value() * 100;
        constexpr auto zero = TileAcc::zero();

        if (speed == TileSpeed::zero())
        {
            return zero;
        }

        if (math::abs(speed.get()) <= min_speed)
        {
            return accelerate_stop(speed);
        }

        auto delta = TileValue::make(-speed.get() / 8);

        return TileAcc::make(delta);
    }


    static TileAcc accelerate_punk_run_x(TileSpeed speed, TickQty32 time)
    {      
        constexpr i32 n_ticks = 60;  
        constexpr i32 max_speed_px = 2;
        constexpr f32 acc_px = (f32)max_speed_px / n_ticks;
        
        constexpr auto run_speed = speed_px(max_speed_px);
        constexpr auto zero = TileAcc::zero();
        constexpr auto acc = accelerate_px(acc_px);

        return speed >= run_speed ? zero : acc;
    }


    static TileAcc accelerate_punk_jump_y(TileSpeed speed, TickQty32 time)
    {


        switch (time.value_)
        {
        case 0: return accelerate_px(0.5f);
        case 1: return accelerate_px(0.5f);
        case 2: return accelerate_px(0.5f);
        case 3: return accelerate_px(0.5f);
        //case 4: return accelerate_px(-0.025f);
        //case 60: return accelerate_stop(speed);
        


        default: return accelerate_px(-0.025f); // gravity
        }        
    }

}


/* acceleration x */

namespace game_punk
{
    static AccelerateFn get_punk_accelerate_x_fn(SpriteMode mode)
    {
        using Mode = SpriteMode;

        switch (mode)
        {
        case Mode::Idle: return accelerate_punk_idle_x;
        case Mode::Run: return accelerate_punk_run_x;

        default: return accelerate_zero;
        }
    }


    static AccelerateFn get_accelerate_x_fn(SpriteName sprite, SpriteMode mode)
    {
        using Sprite = SpriteName;
        using Mode = SpriteMode;

        switch (sprite)
        {
        case Sprite::Punk: return get_punk_accelerate_x_fn(mode);

        default: return accelerate_zero;
        }
    }
}


/* acceleration y */

namespace game_punk
{
    static AccelerateFn get_punk_accelerate_y_fn(SpriteMode mode)
    {
        using Mode = SpriteMode;

        switch (mode)
        {
        case Mode::Jump: return accelerate_punk_jump_y;

        default: return accelerate_zero;
        }
    }


    static AccelerateFn get_accelerate_y_fn(SpriteName sprite, SpriteMode mode)
    {
        using Sprite = SpriteName;
        using Mode = SpriteMode;

        switch (sprite)
        {
        case Sprite::Punk: return get_punk_accelerate_y_fn(mode);

        default: return accelerate_zero;
        }
    }
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
            constexpr auto speed_low_px = 1.0f;

            auto speed_y = vel.y;

            u32 bitmap_id = 0;

            if (speed_y >= TileSpeed::zero())
            {
                if (speed_y >= speed_px(speed_low_px))
                {
                    bitmap_id = 0;
                }
                else
                {
                    bitmap_id = 1;
                }
            }
            else
            {
                if (speed_y >= speed_px(-speed_low_px))
                {
                    bitmap_id = 2;
                }
                else
                {
                    bitmap_id = 3;
                }
            }

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

        SpriteName* name = 0;
        SpriteMode* mode = 0;

        GameTick64* mode_begin = 0;
        GameTick64* tick_end = 0;

        AccelerateFn* accelerate_x = 0;
        AccelerateFn* accelerate_y = 0;

        TileAcc* acceleration_x = 0;
        TileAcc* acceleration_y = 0;

        TileSpeed* speed_x = 0;
        TileSpeed* speed_y = 0;

        TileDim* position_x = 0;
        TileDim* position_y = 0;
        
        AnimateFn* animate = 0;
        BitmapID* bitmap_id = 0;
        
        GameTick64& mode_begin_at(ID id) { return mode_begin[id.value_]; }
        //TileSpeed& velocity_x_at(ID id) { return speed_x[id.value_]; }

        AccelerateFn& accelerate_x_at(ID id) { return accelerate_x[id.value_]; }
        AccelerateFn& accelerate_y_at(ID id) { return accelerate_y[id.value_]; }        
        AnimateFn& animate_at(ID id) { return animate[id.value_]; }

        SpriteName get_name(ID id) const { return name[id.value_]; }
        TileDim get_tile_x(ID id) const { return position_x[id.value_]; }
        VecTile get_tile_pos(ID id) const { return { position_x[id.value_], position_y[id.value_] }; }
        VecSpeed get_tile_velocity(ID id) const { return { speed_x[id.value_], speed_y[id.value_] }; }
    };


    using SpriteID = SpriteTable::ID;

    SpriteID& operator ++ (SpriteID& id) { ++id.value_; return id; }
    bool operator < (SpriteID lhs, SpriteID rhs) { return lhs.value_ < rhs.value_; }
    bool operator == (SpriteID lhs, SpriteID rhs) { return lhs.value_ == rhs.value_; }    


    static void reset_sprite_table(SpriteTable& table)
    {
        table.first_id = 0;

        auto N = table.capacity;

        for (u32 i = 0; i < N; i++)
        {
            table.mode_begin[i] = GameTick64::none();
            table.accelerate_x[i] = accelerate_zero;
            table.accelerate_y[i] = accelerate_zero;
        }
    }


    static void count_table(SpriteTable& table, MemoryCounts& counts, u32 capacity)
    {
        table.capacity = capacity;

        add_count<SpriteName>(counts, capacity);
        add_count<SpriteMode>(counts, capacity);
        
        add_count<GameTick64>(counts, 2 * capacity);

        add_count<AccelerateFn>(counts, 2 * capacity);

        add_count<TileAcc>(counts, 2 * capacity);
        add_count<TileSpeed>(counts, 2 * capacity);
        add_count<TileDim>(counts, 2 * capacity);

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

        auto name = push_mem<SpriteName>(memory, n);
        ok &= name.ok;

        auto mode = push_mem<SpriteMode>(memory, n);
        ok &= mode.ok;

        auto mode_begin = push_mem<GameTick64>(memory, n);
        ok &= mode_begin.ok;

        auto tick_end = push_mem<GameTick64>(memory, n);
        ok &= tick_end.ok;

        auto acc_x_fn = push_mem<AccelerateFn>(memory, n);
        ok &= acc_x_fn.ok;

        auto acc_y_fn = push_mem<AccelerateFn>(memory, n);
        ok &= acc_y_fn.ok;

        auto acc_x = push_mem<TileAcc>(memory, n);
        ok &= acc_x.ok;

        auto acc_y = push_mem<TileAcc>(memory, n);
        ok &= acc_y.ok;

        auto speed_x = push_mem<TileSpeed>(memory, n);
        ok &= speed_x.ok;

        auto speed_y = push_mem<TileSpeed>(memory, n);
        ok &= speed_y.ok;

        auto position_x = push_mem<TileDim>(memory, n);
        ok &= position_x.ok;

        auto position_y = push_mem<TileDim>(memory, n);
        ok &= position_y.ok;

        auto bmp = push_mem<BitmapID>(memory, n);
        ok &= bmp.ok;

        auto animate = push_mem<AnimateFn>(memory, n);
        ok &= animate.ok;

        if (ok)
        {
            table.name = name.data;
            table.mode = mode.data;

            table.mode_begin = mode_begin.data;
            table.tick_end = tick_end.data;

            table.accelerate_x = acc_x_fn.data;
            table.accelerate_y = acc_y_fn.data;

            table.acceleration_x = acc_x.data;
            table.acceleration_y = acc_y.data;

            table.speed_x = speed_x.data;
            table.speed_y = speed_y.data;

            table.position_x = position_x.data;
            table.position_y = position_y.data;

            table.bitmap_id = bmp.data;
            table.animate = animate.data;
        }

        return ok;
    }
        
    
}


/* spawn sprite */

namespace game_punk
{
    class SpriteDef
    {
    public:
        SpriteName name;
        SpriteMode mode;

        GameTick64 mode_begin = GameTick64::none();
        GameTick64 tick_end = GameTick64::forever();

        VecSpeed velocity = vec_zero<TileSpeed>();
        VecTile position = vec_zero<TileDim>();

        BitmapID bitmap_id;

        SpriteDef() = delete;

        SpriteDef(GameTick64 begin, VecTile pos, BitmapID bmp, SpriteName s_name, SpriteMode s_mode)
        {
            name = s_name;
            mode = s_mode;
            mode_begin = begin;
            position = pos;
            bitmap_id = bmp;
        }
    };


    static void despawn_sprite(SpriteTable& table, SpriteID id)
    {
        auto i = id.value_;

        table.mode_begin[i] = GameTick64::none();
        table.accelerate_x[i] = accelerate_zero;
        table.accelerate_y[i] = accelerate_zero;
        table.first_id = math::min(i, table.first_id);
    }


    static bool is_spawned(SpriteTable const& table, SpriteID id)
    {
        return table.mode_begin[id.value_] < table.tick_end[id.value_];
    }    
    
    
    static SpriteID spawn_sprite(SpriteTable& table, SpriteDef const& def)
    {
        auto N = table.capacity;

        SpriteID id;

        u32 i = table.first_id;
        for (; i < N; i++)
        { 
            id = { i };
            if (!is_spawned(table, id))
            {
                break;
            }
        }

        if (i == N)
        {
            i = 0;
        }

        id.value_ = i;
        table.first_id = i;

        table.name[i] = def.name;
        table.mode[i] = def.mode;

        table.mode_begin[i] = def.mode_begin;
        table.tick_end[i] = def.tick_end;

        table.accelerate_x[i] = get_accelerate_x_fn(def.name, def.mode);
        table.accelerate_y[i] = get_accelerate_y_fn(def.name, def.mode);

        table.acceleration_x[i] = TileAcc::zero();
        table.acceleration_y[i] = TileAcc::zero();

        table.speed_x[i] = def.velocity.x;
        table.speed_y[i] = def.velocity.y;

        table.position_x[i] = def.position.x;
        table.position_y[i] = def.position.y;

        table.animate[i] = get_animate_fn(def.name, def.mode);
        table.bitmap_id[i] = def.bitmap_id;

        return id;
    }

}


/* update sprites */

namespace game_punk
{
    static void set_sprite_mode(SpriteTable& table, SpriteID id, SpriteMode mode, GameTick64 tick)
    {
        if (!is_spawned(table, id))
        {
            return;
        }

        auto name = table.get_name(id);

        table.mode_begin_at(id) = tick;
        table.accelerate_x_at(id) = get_accelerate_x_fn(name, mode);
        table.accelerate_y_at(id) = get_accelerate_y_fn(name, mode);
        table.animate_at(id) = get_animate_fn(name, mode);
    }
    
    
    static void move_sprites_x(SpriteTable const& table, GameTick64 tick)
    {
        auto N = table.capacity;

        auto beg = table.mode_begin;

        auto accfn = table.accelerate_x;
        auto acc = table.acceleration_x;
        auto vel = table.speed_x;
        auto pos = table.position_x;

        for (u32 i = 0; i < N; i++)
        {
            auto time = tick - beg[i];

            acc[i] = accfn[i](vel[i], time);
            vel[i] += acc[i];
            pos[i] += vel[i];
        }
    }


    static void move_sprites_y(SpriteTable const& table, GameTick64 tick)
    {
        auto N = table.capacity;

        auto beg = table.mode_begin;

        auto accfn = table.accelerate_y;
        auto acc = table.acceleration_y;
        auto vel = table.speed_y;
        auto pos = table.position_y;

        for (u32 i = 0; i < N; i++)
        {
            auto time = tick - beg[i];

            acc[i] = accfn[i](vel[i], time);
            vel[i] += acc[i];
            pos[i] += vel[i];
        }
    }


    static void move_sprites_xy(SpriteTable const& table, GameTick64 tick)
    {
        auto N = table.capacity;

        auto beg = table.mode_begin;

        auto accfn_x = table.accelerate_x;
        auto accfn_y = table.accelerate_y;

        auto acc_x = table.acceleration_x;
        auto vel_x = table.speed_x;
        auto pos_x = table.position_x;

        auto acc_y = table.acceleration_y;
        auto vel_y = table.speed_y;
        auto pos_y = table.position_y;

        for (u32 i = 0; i < N; i++)
        {
            auto time = tick - beg[i];

            acc_x[i] = accfn_x[i](vel_x[i], time);
            acc_y[i] = accfn_y[i](vel_y[i], time);

            vel_x[i] += acc_x[i];
            vel_y[i] += acc_y[i];

            pos_x[i] += vel_x[i];
            pos_y[i] += vel_y[i];
        }
    }
}
