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
namespace units
{
    class TileAcceleration
    {
    private:
        constexpr TileAcceleration(f32 v) { value_ = v; }

    public:
        f32 value_;

        TileAcceleration() = delete;

        static constexpr TileAcceleration make(f32 v) { return TileAcceleration(v); }
        static constexpr TileAcceleration zero() { return TileAcceleration(0.0f); }
    };


    class TileSpeed
    {
    private:
        constexpr TileSpeed(f32 v) { value_ = v; }

    public:
        f32 value_;

        TileSpeed() = delete;

        TileSpeed operator + (TileSpeed other) { return TileSpeed(value_ + other.value_); }
        TileSpeed operator - (TileSpeed other) { return TileSpeed(value_ - other.value_ ); }

        TileSpeed& operator += (TileAcceleration other) { value_ += other.value_; return *this; }

        static constexpr TileSpeed make(f32 v) { return TileSpeed(v); }
        static constexpr TileSpeed zero() { return TileSpeed(0.0f); }
    };
    
    
    class TileDimension
    {
    private:
        constexpr TileDimension(f32 v) { value_ = v; }
        
        f32 wrap(f32 v)
        {
            v = math::fmod(v, 256.0f);
            v += (v < 0.0f) * 256.0f;
            return v;
        }

    public:    
        f32 value_;

        TileDimension() = delete;

        TileDimension operator + (TileDimension other) { return TileDimension(wrap(value_ + other.value_)); }
        TileDimension operator - (TileDimension other) { return TileDimension(wrap(value_ - other.value_)); }

        TileDimension& operator += (TileSpeed other) { value_ = wrap(value_ + other.value_); return *this; }

        static constexpr TileDimension make(f32 v) { return TileDimension(v); }
        static constexpr TileDimension zero() { return TileDimension(0.0f); }
    };
    
}
}


namespace game_punk
{
namespace units
{
    class GameDimension
    {
    private:
        constexpr GameDimension(i64 v) { value_ = v; }

    public:
        i64 value_;

        GameDimension() = delete;

        GameDimension operator + (GameDimension other) { return GameDimension(value_ + other.value_); }
        GameDimension operator - (GameDimension other) { return GameDimension(value_ - other.value_); }

        static constexpr GameDimension make(i64 v) { return GameDimension(v); }
        static constexpr GameDimension zero() { return GameDimension(0); }
    };


    class SceneDimension
    {
    private:
        constexpr SceneDimension(i32 v) { value_ = v; }

    public:
        i32 value_;

        SceneDimension() = delete;

        SceneDimension operator + (GameDimension other) { return SceneDimension(value_ + other.value_); }
        SceneDimension operator - (GameDimension other) { return SceneDimension(value_ - other.value_); }

        static constexpr SceneDimension make(i32 v) { return SceneDimension(v); }
        static constexpr SceneDimension zero() { return SceneDimension(0); }
    };
}
}