#pragma once

#include "cxpr_math.hpp"


/* abs */

namespace math
{
    f32 abs(f32 num);

    f64 abs(f64 num);    
    
    i32 abs(i32 num);
    
    i8 abs(i8 num);

    i16 abs(i16 num);


    template <typename T>
    inline T abs(T num)
    {
        static_assert(!cxpr::is_unsigned<T>());
        return cxpr::abs(num);
    }
}


/* min */

namespace math
{
    f32 min(f32 a, f32 b);

    f64 min(f64 a, f64 b);

    i8 min(i8 a, i8 b);

    i16 min(i16 a, i16 b);

    i32 min(i32 a, i32 b);

    u8 min(u8 a, u8 b);

    u16 min(u16 a, u16 b);

    u32 min(u32 a, u32 b);


    template <typename T>
    inline T min(T a, T b)
    {
        return cxpr::min(a, b);
    }


    template <typename T>
    inline T min(T a, T b, T c)
    {
        return min(min(a, b), c);
    }
}


/* max */

namespace math
{
    f32 max(f32 a, f32 b);

    f64 max(f64 a, f64 b);

    i8 max(i8 a, i8 b);

    i16 max(i16 a, i16 b);

    i32 max(i32 a, i32 b);

    u8 max(u8 a, u8 b);

    u16 max(u16 a, u16 b);

    u32 max(u32 a, u32 b);


    template <typename T>
    inline T max(T a, T b)
    {
        return cxpr::max(a, b);
    }


    template <typename T>
    inline T max(T a, T b, T c)
    {
        return max(min(a, b), c);
    }
}


/* minmax */

namespace math
{
    template <typename T>
    inline MinMax<T> minmax(T a, T b)
    {
        MinMax<T> mm{};

        mm.min = min(a, b);
        mm.max = max(a, b);

        return mm;
    }


    template <typename T>
    inline MinMax<T> minmax(T a, T b, T c)
    {
        MinMax<T> mm{};

        mm.min = min(a, b, c);
        mm.max = max(a, b, c);

        return mm;
    }
}


/* round, ceil, floor */

namespace math
{
    f32 round(f32 num);

    f64 round(f64 num);

    f32 ceil(f32 num);

    f64 ceil(f64 num);

    f32 floor(f32 num);

    f64 floor(f64 num);
}


/* fma */

namespace math
{
    f32 fma(f32 a, f32 b, f32 c);

    f64 fma(f64 a, f64 b, f64 c);
}


/* fmod */

namespace math
{
    f32 fmod(f32 num, f32 den);

    f64 fmod(f64 num, f64 den);

    f32 modf(f32 num, f32* iptr);

    f64 modf(f64 num, f64* iptr);
}


/* exp, pow, log, sqrt */

namespace math
{
    f32 exp(f32 num);

    f64 exp(f64 num);

    f32 pow(f32 base, f32 exp);

    f64 pow(f64 base, f64 exp);

    f32 log(f32 num);

    f64 log(f64 num);

    f32 log10(f32 num);

    f64 log10(f64 num);

    inline f32 log2(f32 num) { return log(num) / 0.6931472f; }

    inline f64 log2(f64 num) { return log(num) / 0.6931471805599453; }    
}


/* sqrt, rsqrt, hypot */

namespace math
{
    f32 sqrt(f32 num);

    f64 sqrt(f64 num);

    f32 rsqrt(f32 num);

    f64 rsqrt(f64 num);

    f32 hypot(f32 a, f32 b);

    f64 hypot(f64 a, f64 b);

    f32 rhypot(f32 a, f32 b);

    f64 rhypot(f64 a, f64 b);
}


/* trig */

namespace math
{
    f32 sin(f32 rad);

    f64 sin(f64 rad);

    f32 cos(f32 rad);

    f64 cos(f64 rad);

    f32 tan(f32 rad);

    f64 tan(f64 rad);

    f32 asin(f32 sin);

    f64 asin(f64 sin);

    f32 acos(f32 cos);

    f64 acos(f64 cos);

    f32 atan(f32 tan);

    f64 atan(f64 tan);

    f32 atan2(f32 sin, f32 cos);

    f64 atan2(f64 sin, f64 cos);
}


/* trig uangle */

namespace math
{
    inline f32 sin(uangle ua)
    {
        auto rad = cxpr::unsigned_to_rad(ua);

        return sin(rad);
    }


    inline f32 cos(uangle ua)
    {
        auto rad = cxpr::unsigned_to_rad(ua);

        return cos(rad);
    }


    inline uangle atan2_u(f32 sin, f32 cos)
    {
        auto rad = atan2(sin, cos);

        return cxpr::rad_to_unsigned<uangle>(rad);
    }
}


/* random */

namespace math
{
    void rand_init();

    f32 rand(f32 min, f32 max);
}