#pragma once


/* vectors */

namespace game_punk
{
namespace vec
{
    constexpr Vec2Df32 zero_f32 = { 0.0f, 0.0f };
    constexpr Vec2Du32 zero_u32 = { 0, 0 };
    constexpr Vec2Di32 zero_i32 = { 0, 0 };


    static Vec2Df32 add(Vec2Df32 a, Vec2Df32 b)
    {
        return { a.x + b.x, a.y + b.y };
    }


    static Vec2Df32 sub(Vec2Df32 a, Vec2Df32 b)
    {
        return { a.x - b.x, a.y - b.y };
    }


    static Vec2Df32 mul(Vec2Df32 a, f32 scalar)
    {
        return { a.x * scalar, a.y * scalar };
    }


    static f32 dot(Vec2Df32 a, Vec2Df32 b)
    {
        //return a.x * b.x + a.y * b.y;
        return math::fma(a.x, b.x, a.y * b.y);
    }


    static Vec2Df32 unit(Vec2Df32 vec)
    {
        //auto rsqrt = num::rsqrt(vec.x * vec.x + vec.y * vec.y); 
        return mul(vec, math::rhypot(vec.x, vec.y));
    }
}
}


/* tile position */

namespace game_punk
{
namespace tile
{
    class TilePosition
    {
    public:
        Vec2Du32 tile;
        Vec2Df32 offset;
    };


    static void add(u32& tile, f32& offset, f32 delta)
    {
        /*
        f32 delta_tile = 0.0f;
        f32 delta_offset = 0.0f;

        delta_offset = math::modf(delta, &delta_tile);
        */

        f32 delta_tile = math::floor(delta);
        f32 delta_offset = delta - delta_tile;

        tile += (i32)delta_tile;
        offset += delta_offset;

        i32 adj = (offset >= 1.0f);

        offset -= adj;
        tile += adj;
    }


    static inline void sub(u32& tile, f32& offset, f32 delta)
    {
        add(tile, offset, -delta);
    }


    static TilePosition add_tile(TilePosition pos, Vec2Df32 vec_tile)
    {
        add(pos.tile.x, pos.offset.x, vec_tile.x);
        add(pos.tile.y, pos.offset.y, vec_tile.y);

        return pos;
    }


    static TilePosition sub_tile(TilePosition pos, Vec2Df32 vec_tile)
    {
        sub(pos.tile.x, pos.offset.x, vec_tile.x);
        sub(pos.tile.y, pos.offset.y, vec_tile.y);

        return pos;
    }


    static Vec2Df32 delta_tile(TilePosition pos_a, TilePosition pos_b, u32 delta_limit)
    {
        auto delta = pos_a.tile.x - pos_b.tile.x;
        f32 delta_tile_x = delta <= delta_limit ? (f32)delta : -(f32)(pos_b.tile.x - pos_a.tile.x);

        delta = pos_a.tile.y - pos_b.tile.y;
        f32 delta_tile_y = delta <= delta_limit ? (f32)delta : -(f32)(pos_b.tile.y - pos_a.tile.y);

        return {  
            delta_tile_x + pos_a.offset.x - pos_b.offset.x,
            delta_tile_y + pos_a.offset.y - pos_b.offset.y
        };
    }


    constexpr TilePosition zero_pos = { vec::zero_u32, vec::zero_f32 };
}
}