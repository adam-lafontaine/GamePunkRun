#pragma once

#include "cxpr_math.hpp"

#ifndef MATH_NO_SIMD
#define MATH_USE_SIMD
#endif

//#define __AVX__

#ifdef MATH_USE_SIMD

#ifdef __AVX__
#define MATH_SIMD_128
// -mavx -mavx2 -mfma

#include <immintrin.h>

namespace math
{

/* conversions */

namespace avx
{
    static inline __m128 to_128(f32 val32)
    {
        return _mm_set_ss(val32);
    }


    static inline __m128d to_128(f64 val64)
    {
        return _mm_set_sd(val64);
    }

    
    static inline __m128i to_128(i8 val8)
    {
        return _mm_set1_epi8(val8);
    }

    
    static inline __m128i to_128(i16 val16)
    {
        return _mm_set1_epi16(val16);
    }

    
    static inline __m128i to_128(i32 val32)
    {
        return _mm_set1_epi32(val32);
    }
    
    
    static inline __m128i to_128(u8 val8)
    {
        return _mm_set1_epi8(val8);
    }

    
    static inline __m128i to_128(u16 val16)
    {
        return _mm_set1_epi16(val16);
    }

    
    static inline __m128i to_128(u32 val32)
    {
        return _mm_set1_epi32(val32);
    }


    static inline f32 to_f32(__m128 val128)
    {
        return _mm_cvtss_f32(val128);
    }


    static inline f64 to_f64(__m128d val128)
    {
        return _mm_cvtsd_f64(val128);
    }


    static inline i8 to_i8(__m128i val128)
    {
        return _mm_extract_epi8(val128, 0);
    }


    static inline i16 to_i16(__m128i val128)
    {
        return _mm_extract_epi16(val128, 0);
    }


    static inline i32 to_i32(__m128i val128)
    {
        return _mm_extract_epi32(val128, 0);
    }


    static inline u8 to_u8(__m128i val128)
    {
        return static_cast<u8>(_mm_extract_epi8(val128, 0));
    }


    static inline u16 to_u16(__m128i val128)
    {
        return static_cast<u16>(_mm_extract_epi16(val128, 0));
    }


    static inline u32 to_u32(__m128i val128)
    {
        return static_cast<u32>(_mm_extract_epi32(val128, 0));
    }
}


/* min */

namespace avx
{
    static inline f32 min(f32 a, f32 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_min_ss(a128, b128);

        return avx::to_f32(res);
    }


    static inline f64 min(f64 a, f64 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_min_sd(a128, b128);

        return avx::to_f64(res);
    }


    static inline i8 min(i8 a, i8 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_min_epi8(a128, b128);

        return avx::to_i8(res);
    }


    static inline i16 min(i16 a, i16 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_min_epi16(a128, b128);

        return avx::to_i16(res);
    }


    static inline i32 min(i32 a, i32 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_min_epi32(a128, b128);

        return avx::to_i32(res);
    }


    static inline u8 min(u8 a, u8 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_min_epu8(a128, b128);

        return avx::to_u8(res);
    }


    static inline u16 min(u16 a, u16 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_min_epu16(a128, b128);

        return avx::to_u16(res);
    }


    static inline u32 min(u32 a, u32 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_min_epu32(a128, b128);

        return avx::to_u32(res);
    }
}


/* max */

namespace avx
{
    static inline f32 max(f32 a, f32 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_max_ss(a128, b128);

        return avx::to_f32(res);
    }


    static inline f64 max(f64 a, f64 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_max_sd(a128, b128);

        return avx::to_f64(res);
    }


    static inline i8 max(i8 a, i8 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_max_epi8(a128, b128);

        return avx::to_i8(res);
    }


    static inline i16 max(i16 a, i16 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_max_epi16(a128, b128);

        return avx::to_i16(res);
    }


    static inline i32 max(i32 a, i32 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_max_epi32(a128, b128);

        return avx::to_i32(res);
    }


    static inline u8 max(u8 a, u8 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_max_epu8(a128, b128);

        return avx::to_u8(res);
    }


    static inline u16 max(u16 a, u16 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_max_epu16(a128, b128);

        return avx::to_u16(res);
    }


    static inline u32 max(u32 a, u32 b)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);

        auto res = _mm_max_epu32(a128, b128);

        return avx::to_u32(res);
    }
}


/* fma */

namespace avx
{
    static inline f32 fma(f32 a, f32 b, f32 c)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);
        auto c128 = avx::to_128(c);

        auto res = _mm_fmadd_ss(a128, b128, c128);

        return avx::to_f32(res);
    }


    static inline f64 fma(f64 a, f64 b, f64 c)
    {
        auto a128 = avx::to_128(a);
        auto b128 = avx::to_128(b);
        auto c128 = avx::to_128(c);

        auto res = _mm_fmadd_sd(a128, b128, c128);

        return avx::to_f64(res);
    }
}


/* ceil, floor */

namespace avx
{
    static inline f32 ceil(f32 num)
    {
        auto v = avx::to_128(num);
        auto res = _mm_ceil_ps(v);

        return avx::to_f32(res);
    }


    static inline f32 floor(f32 num)
    {
        auto v = avx::to_128(num);
        auto res = _mm_floor_ps(v);

        return avx::to_f32(res);
    }
}


/* sqrt, rsqrt */

namespace avx
{
    static inline f32 sqrt(f32 num)
    {
        auto v = avx::to_128(num);
        auto res = _mm_sqrt_ss(v);

        return avx::to_f32(res);
    }


    static inline f64 sqrt(f64 num)
    {        
        auto v1 = avx::to_128(num);
        auto v2 = avx::to_128(num);
        auto res = _mm_sqrt_sd(v1, v2);

        return avx::to_f64(res);
    }


    static inline f32 rsqrt(f32 num)
    {
        auto num128 = avx::to_128(num);
        auto res = _mm_rsqrt_ss(num128);

        return avx::to_f32(res);
    }
}

} // math

#endif // __AVX__

#endif // MATH_USE_SIMD
