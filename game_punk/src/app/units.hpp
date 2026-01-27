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

        f32 get() { return value_; }
    };


    using TileDelta = TileSpeed;


    class TileDimension
    {
    private:
        
        class ValueT
        {
        public:
            u32 i_part;
            f32 f_part;

            constexpr ValueT(){}
        };

        ValueT value_;

        constexpr TileDimension(u32 i, f32 f) 
        { 
            auto adj = f < 0.0f;
            value_.i_part = i - adj;
            value_.f_part = f + adj;
        }


        void add(f32 delta)
        {
            auto f = value_.f_part + delta;
            f32 i;
            f = math::modf(f, &i);
            auto adj = f < 0.0f;
            value_.i_part += (i32)i - adj;
            value_.f_part = f + adj;
        }

    public:

        TileDimension() = delete;

        TileDimension& operator += (TileSpeed other) { add(other.value_); return *this; }
        TileDimension& operator -= (TileSpeed other) { add(-other.value_); return *this; }

        TileDelta operator - (TileDimension other) 
        {
            auto i = (f32)value_.i_part - (f32)other.value_.i_part;
            auto f = value_.f_part - other.value_.f_part;

            return TileDelta::make(i + f);
        }

        static constexpr TileDimension make(u32 v) 
        {
            return TileDimension(v, 0.0f);
        }

        static TileDimension zero() { return make(0.0f); }

        //i64 convert_i(u32 len) { return len * value_.i_part; }
        //i64 convert_f(u32 len) { return (i64)(len * value_.f_part); }

        f32 get() { return (f32)value_.i_part + value_.f_part; }
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

        GameDimension operator + (GameDimension other) const { return GameDimension(value_ + other.value_); }
        GameDimension operator - (GameDimension other) const { return GameDimension(value_ - other.value_); }

        GameDimension operator += (GameDimension other) { value_ += other.value_; return *this; }

        static constexpr GameDimension make(i64 v) { return GameDimension(v); }
        static constexpr GameDimension zero() { return GameDimension(0); }

        i64 get() { return value_; }
    };


    class SceneDimension
    {
    private:
        constexpr SceneDimension(i32 v) { value_ = v; }

    public:
        i32 value_;

        SceneDimension() = delete;

        SceneDimension operator + (SceneDimension other) { return SceneDimension(value_ + other.value_); }
        SceneDimension operator - (SceneDimension other) { return SceneDimension(value_ - other.value_); }

        SceneDimension operator += (SceneDimension other) { value_ += other.value_; return *this; }

        static constexpr SceneDimension make(i32 v) { return SceneDimension(v); }
        static constexpr SceneDimension zero() { return SceneDimension(0); }

        i32 get() { return value_; }
    };


    static i32 delta_i32(SceneDimension a, SceneDimension b) { return a.value_ - b.value_; }
}
}