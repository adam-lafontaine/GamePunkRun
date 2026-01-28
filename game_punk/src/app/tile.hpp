#pragma once


/* tile state */

namespace game_punk
{
    class TileState
    {
    public:
        TileView floor_a;
        TileView floor_b;
    };


    static void count_tile_state(TileState& tiles, MemoryCounts& counts)
    {
        using Ex = bt::Tileset_ex_zone;

        constexpr Ex list;
        constexpr auto f2 = Ex::Items::floor_02;
        constexpr auto f3 = Ex::Items::floor_03;

        count_view(tiles.floor_a, counts, bt::item_at(list, f2));
        count_view(tiles.floor_b, counts, bt::item_at(list, f3));
    }


    static bool create_tile_state(TileState& tiles, Memory& memory)
    {
        bool ok = true;

        ok &= create_view(tiles.floor_a, memory);
        ok &= create_view(tiles.floor_b, memory);

        return ok;
    }
}


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
        VecTile* position = 0;
        BitmapID* bitmap_id = 0;
    };


    using TileID = TileTable::ID;

    TileID& operator ++ (TileID& id) { ++id.value_; return id; }
    bool operator < (TileID lhs, TileID rhs) { return lhs.value_ < rhs.value_; }


    class TileDef
    {
    public:
        GameTick64 tick_begin = GameTick64::none();
        VecTile position = vec_zero<TileDim>();
        BitmapID bitmap_id;

        TileDef() = delete;

        TileDef(GameTick64 begin, VecTile pos, BitmapID bmp)
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
        add_count<VecTile>(counts, capacity);
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

        auto position = push_mem<VecTile>(memory, n);
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
    
    
    static void despawn_tile(TileTable& table, TileID id)
    {
        auto i = id.value_;

        table.tick_begin[i] = GameTick64::none();
        table.first_id = math::min(i, table.first_id);
    }


    static bool is_spawned(TileTable const& table,TileID id)
    {
        return table.tick_begin[id.value_] != GameTick64::none();
    }
    
    
    static TileID spawn_tile(TileTable& table, TileDef const& tile)
    {
        auto N = table.capacity;

        TileID id;
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
        
        table.first_id = i;
        table.tick_begin[i] = tile.tick_begin;
        table.position[i] = tile.position;
        table.bitmap_id[i] = tile.bitmap_id;

        return id;
    }
    
}