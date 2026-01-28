#pragma once


/* tile position */

namespace game_punk
{
namespace units
{
    constexpr i32 BASE_TILE_VALUE = 100000;


    class TileValue
    {
    private:
        constexpr TileValue(f32 v) { value_ = (i32(v * BASE_TILE_VALUE)); }

        constexpr TileValue(i32 v) { value_ = v; }

    public:

        i32 value_;

        constexpr TileValue(){}

        static constexpr TileValue make(f32 v) { return TileValue(v); }
        static constexpr TileValue make(i32 v) { return TileValue(v); }
        static constexpr TileValue zero() { return TileValue(0); }

        TileValue make(u32 v) = delete;

        TileValue operator + (TileValue other) { return TileValue(value_ + other.value_); }
        TileValue operator - (TileValue other) { return TileValue(value_ - other.value_); }
        
    };


    class TileAcceleration
    {
    private:
        constexpr TileAcceleration(TileValue v) { value_ = v; }

    public:
        TileValue value_;

        TileAcceleration() = delete;

        static constexpr TileAcceleration make(TileValue v) { return TileAcceleration(v); }
        static constexpr TileAcceleration zero() { return TileAcceleration(TileValue::zero()); }

        static TileAcceleration make(f32 v) = delete;

        f32 get() { return (f32)value_.value_ / BASE_TILE_VALUE; }
    };


    class TileSpeed
    {
    private:
        constexpr TileSpeed(TileValue v) { value_ = v; }

    public:

        TileValue value_;

        TileSpeed() = delete;

        static constexpr TileSpeed make(TileValue v) { return TileSpeed(v); }
        static constexpr TileSpeed zero() { return TileSpeed(TileValue::zero()); }

        static TileSpeed make(f32 v) = delete;

        TileSpeed operator + (TileSpeed other) { return TileSpeed(value_ + other.value_); }
        TileSpeed operator - (TileSpeed other) { return TileSpeed(value_ - other.value_ ); }

        TileSpeed& operator += (TileAcceleration other) { value_.value_ += other.value_.value_; return *this; }

        bool operator == (TileSpeed other) { return value_.value_ == other.value_.value_; }
        bool operator >= (TileSpeed other) { return value_.value_ >= other.value_.value_; }

        f32 get() { return (f32)value_.value_ / BASE_TILE_VALUE; }
    };


    using TileDelta = TileSpeed;


    class TileDimension
    {
    private:
        
        class ValueT
        {
        public:
            u32 i_part;
            i32 f_part;

            constexpr ValueT(){}
        };

        ValueT value_;

        constexpr TileDimension(u32 i, i32 f) 
        { 
            i += (f / BASE_TILE_VALUE);
            f %= BASE_TILE_VALUE;

            i32 adj = (f < 0);
            value_.i_part = i - adj;
            value_.f_part = f + adj * BASE_TILE_VALUE;
        }


        void add(i32 f_delta)
        {
            u32 i = value_.i_part;
            i32 f = value_.f_part + f_delta;

            i += (f / BASE_TILE_VALUE);
            f %= BASE_TILE_VALUE;
            
            i32 adj = (f < 0);
            value_.i_part = i - adj;
            value_.f_part = f + adj * BASE_TILE_VALUE;
        }

    public:

        TileDimension() = delete;

        static constexpr TileDimension make(TileValue v) { return TileDimension(v.value_, 0); }
        static constexpr TileDimension zero() { return TileDimension(0u, 0); }

        static TileDimension make(f32 v) = delete;

        TileDimension& operator += (TileSpeed other) { add(other.value_.value_); return *this; }
        TileDimension& operator -= (TileSpeed other) { add(-other.value_.value_); return *this; }

        TileDelta operator - (TileDimension other) 
        {
            u32 i = value_.i_part - other.value_.i_part;
            i32 f = value_.f_part - other.value_.f_part;

            i32 n = i * BASE_TILE_VALUE + f;

            return TileDelta::make(TileValue::make(n));
        }

        f32 get() { return (f32)value_.i_part + (f32)value_.f_part / BASE_TILE_VALUE; }
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