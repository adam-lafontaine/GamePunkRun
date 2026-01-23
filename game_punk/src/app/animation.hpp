#pragma once





/* sprite animation */

/*namespace game_punk
{
    using AnimationFn = u32 (*)(Vec2Di32 vel);


    template <u32 N>
    static u32 constant_ticks(Vec2Di32 vel)
    {
        return N;
    }
    
    
    static u32 punk_run_ticks(Vec2Di32 vel)
    {
        auto x = (u32)math::cxpr::clamp(vel.x, 1, 10);
        return 10u / x;
    }


    static u32 punk_jump_ticks(Vec2Di32 vel)
    {

    }
    
    
    class SpriteAnimation
    {
    public:
        AnimationFn get_ticks;

        ContextDims bitmap_dims;
        u32 bitmap_count = 0;
        p32* spritesheet_data = 0;
    };


    static bool x_init_animation(SpriteAnimation& an, SpritesheetView const& ss, AnimationFn ticks_fn)
    {
        auto& dims = ss.dims.game;

        bool ok = ss.data != 0;
        app_assert(ok && "*** Spritesheet data not set ***");

        ok &= dims.width > dims.height;
        ok &= dims.width % dims.height == 0;
        app_assert(ok && "*** Invalid spritesheet dimensions ***");

        an.spritesheet_data = ss.data;
        an.bitmap_dims = ss.bitmap_dims;
        an.bitmap_count = ss.bitmap_count;

        an.get_ticks = ticks_fn;

        return ok;
    }


    static SpriteView get_animation_bitmap(SpriteAnimation& an, Vec2Di32 vel, TickQty32 time)
    {
        p32* data = 0;

        SpriteView view;
        view.dims = an.bitmap_dims;

        auto dims = an.bitmap_dims.proc;

        auto ticks = an.get_ticks(vel);

        auto t = time.value_ % (an.bitmap_count * ticks);
        auto b = t / ticks;

        auto offset = b * dims.width * dims.height;
        
        data = an.spritesheet_data + offset;

        app_assert(data);
        
        view.data = data;

        return view;
    }

}


namespace game_punk
{
    //using AnimationTable = ObjectTable<SpriteAnimation>;
    //using AnimationID = AnimationTable::ID;
}*/






