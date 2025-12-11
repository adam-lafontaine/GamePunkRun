#include "math.hpp"
#include "math_intrin.hpp"

#include <cstdlib>
#include <cmath>

#define MATH_AVOID_CMATH 1


/* abs */

namespace math
{
    f32 abs(f32 num) 
    { 
        // Mask: 0x7FFFFFFF = clear sign bit
        constexpr i32 m32 = 0x7FFFFFFF;

    #ifdef MATH_SIMD_128       

        auto v128 = avx::to_128(num);
        
        auto mask = _mm_castsi128_ps(_mm_set1_epi32(m32));
        auto res = _mm_and_ps(v128, mask);

        return avx::to_f32(res);

    #elif MATH_AVOID_CMATH

        union
        {
            f32 f;
            i32 i;
        } conv;

        conv.f = num;
        conv.i &= m32;

        return conv.f;

    #else
        return std::abs(num);
    #endif
    }


    f64 abs(f64 num) 
    { 
        // Mask: 0x7FFFFFFFFFFFFFFF = clear sign bit
        constexpr i64 m64 = 0x7FFFFFFFFFFFFFFF;

    #ifdef MATH_SIMD_128

        auto v128 = avx::to_128(num);
        
        auto mask = _mm_castsi128_pd(_mm_set1_epi64x(m64));
        auto res = _mm_and_pd(v128, mask);

        return avx::to_f64(res);

    #elif MATH_AVOID_CMATH

        union
        {
            f64 f;
            i64 i;
        } conv;

        conv.f = num;
        conv.i &= m64;

        return conv.f;

    #else
        return std::abs(num);
    #endif
    }


    i32 abs(i32 num) 
    { 
    #ifdef MATH_SIMD_128

        auto v128 = avx::to_128(num);
        auto res = _mm_abs_epi32(v128);

        return avx::to_i32(res);

    #else
        return std::abs(num);
    #endif
    }


    i8 abs(i8 num)
    {
    #ifdef MATH_SIMD_128

        auto v128 = avx::to_128(num);
        auto res = _mm_abs_epi8(v128);

        return avx::to_i8(res);

    #else
        return (i8)std::abs(num);
    #endif
    }


    i16 abs(i16 num)
    {
    #ifdef MATH_SIMD_128

        auto v128 = avx::to_128(num);
        auto res = _mm_abs_epi16(v128);

        return avx::to_i16(res);

    #else
        return (i16)std::abs(num);
    #endif
    }
}


/* min */

namespace math
{
    f32 min(f32 a, f32 b)
    {
    #ifdef MATH_SIMD_128
        return avx::min(a, b);
    #else
        return std::min(a, b);
    #endif
    }

    
    f64 min(f64 a, f64 b)
    {
    #ifdef MATH_SIMD_128
        return avx::min(a, b);
    #else
        return std::min(a, b);
    #endif
    }


    i8 min(i8 a, i8 b)
    {
    #ifdef MATH_SIMD_128
        return avx::min(a, b);
    #else
        return std::min(a, b);
    #endif
    }


    i16 min(i16 a, i16 b)
    {
    #ifdef MATH_SIMD_128
        return avx::min(a, b);
    #else
        return std::min(a, b);
    #endif
    }


    i32 min(i32 a, i32 b)
    {
    #ifdef MATH_SIMD_128
        return avx::min(a, b);
    #else
        return std::min(a, b);
    #endif
    }


    u8 min(u8 a, u8 b)
    {
    #ifdef MATH_SIMD_128
        return avx::min(a, b);
    #else
        return std::min(a, b);
    #endif
    }


    u16 min(u16 a, u16 b)
    {
    #ifdef MATH_SIMD_128
        return avx::min(a, b);
    #else
        return std::min(a, b);
    #endif
    }


    u32 min(u32 a, u32 b)
    {
    #ifdef MATH_SIMD_128
        return avx::min(a, b);
    #else
        return std::min(a, b);
    #endif
    }
}


/* max */

namespace math
{
    f32 max(f32 a, f32 b)
    {
    #ifdef MATH_SIMD_128
        return avx::max(a, b);
    #else
        return std::max(a, b);
    #endif
    }

    
    f64 max(f64 a, f64 b)
    {
    #ifdef MATH_SIMD_128
        return avx::max(a, b);
    #else
        return std::max(a, b);
    #endif
    }


    i8 max(i8 a, i8 b)
    {
    #ifdef MATH_SIMD_128
        return avx::max(a, b);
    #else
        return std::max(a, b);
    #endif
    }


    i16 max(i16 a, i16 b)
    {
    #ifdef MATH_SIMD_128
        return avx::max(a, b);
    #else
        return std::max(a, b);
    #endif
    }


    i32 max(i32 a, i32 b)
    {
    #ifdef MATH_SIMD_128
        return avx::max(a, b);
    #else
        return std::max(a, b);
    #endif
    }


    u8 max(u8 a, u8 b)
    {
    #ifdef MATH_SIMD_128
        return avx::max(a, b);
    #else
        return std::max(a, b);
    #endif
    }


    u16 max(u16 a, u16 b)
    {
    #ifdef MATH_SIMD_128
        return avx::max(a, b);
    #else
        return std::max(a, b);
    #endif
    }


    u32 max(u32 a, u32 b)
    {
    #ifdef MATH_SIMD_128
        return avx::max(a, b);
    #else
        return std::max(a, b);
    #endif
    }
}


/* round, ceil, floor */

namespace math
{
    f32 round(f32 num) { return std::roundf(num); }

    f64 round(f64 num) { return std::round(num); }


    f32 ceil(f32 num) 
    {
    #ifdef MATH_SIMD_128
        return avx::ceil(num);
    #else
        return std::ceil(num);
    #endif
    }


    f64 ceil(f64 num) { return std::ceil(num); }


    f32 floor(f32 num) 
    { 
    #ifdef MATH_SIMD_128
        return avx::floor(num);
    #else
        return std::floor(num);
    #endif
    }


    f64 floor(f64 num) { return std::floor(num); }
}


/* fma */

namespace math
{
    f32 fma(f32 a, f32 b, f32 c)
    {
    #ifdef MATH_SIMD_128
        return avx::fma(a, b, c);
    #elif MATH_AVOID_CMATH
        return a * b + c;
    #else
        return std::fmaf(a, b, c);
    #endif
    }


    f64 fma(f64 a, f64 b, f64 c)
    {
    #ifdef MATH_SIMD_128
        return avx::fma(a, b, c);
    #elif MATH_AVOID_CMATH
        return a * b + c;
    #else
        return std::fma(a, b, c);
    #endif
    }
}


/* fmod */

namespace math
{
    f32 fmod(f32 num, f32 den) { return std::fmod(num, den); }

    f64 fmod(f64 num, f64 den) { return std::fmod(num, den); }

    f32 modf(f32 num, f32* iptr) { return std::modf(num, iptr); }

    f64 modf(f64 num, f64* iptr) { return std::modf(num, iptr); }
}


/* exp, pow, log */

namespace math
{
    f32 exp(f32 num) { return std::exp(num); }

    f64 exp(f64 num) { return std::exp(num); }

    f32 pow(f32 base, f32 exp) { return std::pow(base, exp); }

    f64 pow(f64 base, f64 exp) { return std::pow(base, exp); }

    f32 log(f32 num) { return std::log(num); }

    f64 log(f64 num) { return std::log(num); }

    f32 log10(f32 num) { return std::log10(num); }

    f64 log10(f64 num) { return std::log10(num); }    
}


/* sqrt, rsqrt, hypot */

namespace math
{    
    static f32 quick_rsqrt(f32 num)
    {
        union
        {
            f32 f;
            i32 i;
        } conv;

        static_assert(sizeof(f32) == 4);
        static_assert(sizeof(f32) == sizeof(i32));

        conv.f = num;
        conv.i = 0x5f3759df - (conv.i >> 1);

        f32 y = conv.f;        
        f32 x2 = num * 0.5f;

        y = y * (1.5f - (x2 * y * y)); // Newton-Raphson iteration

        return y;
    }


    f32 sqrt(f32 num)
    { 
        if (num <= 0.0f)
        {
            return 0.0f;
        }

    #ifdef MATH_SIMD_128
        return avx::sqrt(num);
    #elif MATH_AVOID_CMATH
        return 1.0f / quick_rsqrt(num);
    #else
        return std::sqrt(num);
    #endif
    }


    f64 sqrt(f64 num)
    { 
        if (num <= 0.0)
        {
            return 0.0;
        }

    #ifdef MATH_SIMD_128
        return avx::sqrt(num);
    #else
        return std::sqrt(num);
    #endif
    }


    f32 rsqrt(f32 num)
    {
        if (num <= 0.0f)
        {
            return 0.0f;
        }

    #ifdef MATH_SIMD_128
        return avx::sqrt(num);
    #elif MATH_AVOID_CMATH
        return quick_rsqrt(num);
    #else
        return 1.0f / std::sqrt(num);
    #endif
    }


    f64 rsqrt(f64 num)
    { 
        if (num <= 0.0)
        {
            return 0.0;
        }

    #ifdef MATH_SIMD_128
        return avx::rsqrt((f32)num);        
    #else
        return 1.0 / std::sqrt(num);
    #endif
    }


    f32 hypot(f32 a, f32 b)
    {
    #if MATH_AVOID_CMATH
        return 1.0f / quick_rsqrt(fma(a, a, b * b));
    #else
        return std::hypotf(a, b);
    #endif
    }


    f64 hypot(f64 a, f64 b)
    {
    #if MATH_AVOID_CMATH
        return sqrt(fma(a, a, b * b));
    #else
        return std::hypot(a, b);
    #endif
    }


    f32 rhypot(f32 a, f32 b)
    {
    #if MATH_AVOID_CMATH
        return quick_rsqrt(fma(a, a, b * b));
    #else
        return 1.0f / std::hypotf(a, b);
    #endif
    }


    f64 rhypot(f64 a, f64 b)
    {
        return 1.0 / std::hypot(a, b);
    }
}


/* trig */

namespace math
{
    f32 sin(f32 rad) { return std::sin(rad); }

    f64 sin(f64 rad) { return std::sin(rad); }

    f32 cos(f32 rad) { return std::cos(rad); }

    f64 cos(f64 rad) { return std::cos(rad); }

    f32 tan(f32 rad) { return std::tan(rad); }

    f64 tan(f64 rad) { return std::tan(rad); }

    f32 asin(f32 sin) { return std::asin(sin); }

    f64 asin(f64 sin) { return std::asin(sin); }

    f32 acos(f32 cos) { return std::acos(cos); }

    f64 acos(f64 cos) { return std::acos(cos); }

    f32 atan(f32 tan) { return std::atan(tan); }

    f64 atan(f64 tan) { return std::atan(tan); }

    f32 atan2(f32 sin, f32 cos) { return std::atan2(sin, cos); }

    f64 atan2(f64 sin, f64 cos) { return std::atan2(sin, cos); }
}


/* random */

namespace math
{
    void rand_init()
    {
        std::srand(1);
    }


    f32 rand(f32 min, f32 max)
    {
        auto r = (f32)std::rand() / (f32)RAND_MAX;
        return min + r * (max - min);
    }
}