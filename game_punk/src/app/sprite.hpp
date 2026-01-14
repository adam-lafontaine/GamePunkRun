/* tile soa */

namespace game_punk
{
    class TileTable
    {
    public:
        struct ID { u32 value_ = 0; };

        u32 capacity = 0;
        ID first_id = {0};

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
        table.first_id = {0};

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


    static TileID spawn_tile(TileTable& table, TileDef const& tile)
    {
        auto beg = table.tick_begin;
        auto available = GameTick64::none();

        TileID id = table.first_id;
        TileID end = {table.capacity};
        for (; id < end && beg[id.value_] != available; ++id)
        { }
        
        table.first_id = id;
        table.tick_begin[id.value_] = tile.tick_begin;
        table.position[id.value_] = tile.position;
        table.bitmap_id[id.value_] = tile.bitmap_id;

        return id;
    }


    static void despawn_tile(TileTable& table, TileID id)
    {
        table.tick_begin[id.value_] = GameTick64::none();
        table.first_id = math::min(id, table.first_id);
    }
}


/* sprite soa */

namespace game_punk
{
    class SpriteSOA
    {
    public:
        u32 capacity = 0;
        u32 first_id = 0;

        GameTick64* tick_begin = 0;
        GameTick64* tick_end = 0;
        
        Vec2Di32* velocity_px = 0;
        Vec2Di64* position = 0;

        SpriteAnimation* animations = 0;

        BitmapID* bitmap_id = 0;
    };
}