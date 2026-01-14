/* tile soa */

namespace game_punk
{
    class TileTable
    {
    public:
        struct ID { u32 value_ = 0; };

        u32 capacity = 0;
        u32 first_id = {0};

        GameTick64* tick_begin = 0;

        Vec2Di64* position = 0;

        BitmapID* bitmap_id = 0;
    };


    using TileID = TileTable::ID;

    TileID& operator ++ (TileID& id) { ++id.value_; return id; }
    bool operator < (TileID lhs, TileID rhs) { return lhs.value_ < rhs.value_; }


    class TileDef
    {
    public:
        GameTick64 tick_begin = GameTick64::none();
        Vec2D<i64> position;
        BitmapID bitmap_id;

        TileDef() = delete;

        TileDef(GameTick64 begin, Vec2D<i64> pos, BitmapID bmp)
        {
            tick_begin = begin;
            position = pos;
            bitmap_id = bmp;
        }
    };


    static void reset_tile_table(TileTable& table)
    {
        table.first_id = 0;

        span::fill(span::make_view(table.tick_begin, table.capacity), GameTick64::none());
    }


    static void count_table(TileTable& table, MemoryCounts& counts, u32 capacity)
    {
        table.capacity = capacity;

        add_count<GameTick64>(counts, capacity);
        add_count<Vec2Di64>(counts, capacity);
        add_count<BitmapID>(counts, capacity);
    }


    static bool create_table(TileTable& table, Memory& memory)
    {
        if (!table.capacity)
        {
            app_crash("*** TileTable not initialized ***");
            return false;
        }

        auto n = table.capacity;

        bool ok = true;

        auto tick_begin = push_mem<GameTick64>(memory, n);
        ok &= tick_begin.ok;

        auto position = push_mem<Vec2Di64>(memory, n);
        ok &= position.ok;

        auto bmp = push_mem<BitmapID>(memory, n);
        ok &= bmp.ok;

        if (ok)
        {
            table.tick_begin = tick_begin.data;
            table.position = position.data;
            table.bitmap_id = bmp.data;
        }

        return ok;
    }


    static void despawn_tile(TileTable& table, u32 i)
    {
        table.tick_begin[i] = GameTick64::none();
        table.first_id = math::min(i, table.first_id);
    }
    
    
    static void despawn_tile(TileTable& table, TileID id)
    {
        despawn_tile(table, id.value_);
    }


    static bool is_spawned(TileTable const& table, u32 i)
    {
        return table.tick_begin[i] != GameTick64::none();
    }
    
    
    static TileID spawn_tile(TileTable& table, TileDef const& tile)
    {
        auto N = table.capacity;

        TileID id;
        u32 i = table.first_id;
        for (; is_spawned(table, i) && i < N; i++)
        { }

        if (i == N)
        {
            i = 0;
        }
        
        table.first_id = i;
        table.tick_begin[i] = tile.tick_begin;
        table.position[i] = tile.position;
        table.bitmap_id[i] = tile.bitmap_id;

        return id;
    }
    
}


/* sprite soa */

/*namespace game_punk
{
    class SpriteTable
    {
    public:
        struct ID { u32 value_ = 0; };

        u32 capacity = 0;
        u32 first_id = 0;

        GameTick64* tick_begin = 0;
        GameTick64* tick_end = 0;
        
        Vec2Di32* velocity_px = 0;
        Vec2Di64* position = 0;

        SpriteAnimation* animation = 0;

        BitmapID* bitmap_id = 0;
        
        GameTick64& tick_begin_at(ID id) { return tick_begin[id.value_]; }
        //GameTick64& tick_end_at(ID id) { return tick_end[id.value_]; }
        Vec2Di64& position_at(ID id) { return position[id.value_]; }
        Vec2Di32& velocity_px_at(ID id) { return velocity_px[id.value_]; }
        SpriteAnimation& animation_at(ID id) { return animation[id.value_]; }
    };


    using SpriteID = SpriteTable::ID;

    SpriteID& operator ++ (SpriteID& id) { ++id.value_; return id; }
    bool operator < (SpriteID lhs, SpriteID rhs) { return lhs.value_ < rhs.value_; }


    class SpriteDef
    {
    public:
        GameTick64 tick_begin = GameTick64::none();
        GameTick64 tick_end = GameTick64::forever();
        Vec2D<i64> position;

        Vec2D<i32> velocity;
        BitmapID bitmap_id;


        SpriteDef() = delete;

        SpriteDef(GameTick64 begin, Vec2D<i64> pos, BitmapID bmp)
        {
            tick_begin = begin;
            position = pos;
            bitmap_id = bmp;
            velocity = {0};
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
        add_count<Vec2Di64>(counts, capacity);
        add_count<Vec2Di32>(counts, capacity);
        add_count<BitmapID>(counts, capacity);
        add_count<SpriteAnimation>(counts, capacity);
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

        auto position = push_mem<Vec2Di64>(memory, n);
        ok &= position.ok;

        auto velocity = push_mem<Vec2Di32>(memory, n);
        ok &= velocity.ok;

        auto bmp = push_mem<BitmapID>(memory, n);
        ok &= bmp.ok;

        auto animation = push_mem<SpriteAnimation>(memory, n);
        ok &= animation.ok;

        if (ok)
        {
            table.tick_begin = tick_begin.data;
            table.tick_end = tick_end.data;
            table.position = position.data;
            table.velocity_px = velocity.data;
            table.bitmap_id = bmp.data;
            table.animation = animation.data;
        }

        return ok;
    }


    static SpriteID spawn_sprite(SpriteTable& table, SpriteDef const& def)
    {
        auto beg = table.tick_begin;
        auto end = table.tick_end;

        SpriteID id;

        u32 i = table.first_id;
        for (; i < table.capacity && beg[i] < end[i]; i++)
        { }

        id.value_ = i;
        table.first_id = i;
        table.tick_begin[i] = def.tick_begin;
        table.tick_end[i] = def.tick_end;
        table.position[i] = def.position;
        table.velocity_px[i] = def.velocity;
        table.bitmap_id[i] = def.bitmap_id;

        return id;
    }


    static void despawn_sprite(SpriteTable& table, SpriteID id)
    {
        table.tick_begin_at(id) = GameTick64::none();
        table.first_id = math::min(id.value_, table.first_id);
    }


    static void move_sprites(SpriteTable const& table)
    {
        auto beg = table.tick_begin;
        auto end = table.tick_end;
        auto pos = table.position;
        auto vel = table.velocity_px;

        for (u32 i = 0; i < table.capacity; i++)
        {
            pos[i].x += vel[i].x;
            pos[i].y += vel[i].y;
        }
    }
}*/