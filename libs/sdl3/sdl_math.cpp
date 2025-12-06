#include "../math/math.hpp"
#include "../math/math_intrin.hpp"

#include <SDL3/SDL.h>


/* abs */

namespace math
{
    f32 abs(f32 num) { return SDL_fabsf(num); }

    f64 abs(f64 num) { return SDL_fabs(num); }

    i32 abs(i32 num) { return SDL_abs(num); }
    
    i8 abs(i8 num) { return (i8)SDL_abs(num); }

    i16 abs(i16 num) { return (i16)SDL_abs(num); }
}


/* min */

namespace math
{
    f32 min(f32 a, f32 b) { return SDL_min(a, b); }

    f64 min(f64 a, f64 b) { return SDL_min(a, b); }

    i8 min(i8 a, i8 b) { return SDL_min(a, b); }

    i16 min(i16 a, i16 b) { return SDL_min(a, b); }

    i32 min(i32 a, i32 b) { return SDL_min(a, b); }

    u8 min(u8 a, u8 b) { return SDL_min(a, b); }

    u16 min(u16 a, u16 b) { return SDL_min(a, b); }

    u32 min(u32 a, u32 b) { return SDL_min(a, b); }
}


/* max */

namespace math
{
    f32 max(f32 a, f32 b) { return SDL_max(a, b); }

    f64 max(f64 a, f64 b) { return SDL_max(a, b); }

    i8 max(i8 a, i8 b) { return SDL_max(a, b); }

    i16 max(i16 a, i16 b) { return SDL_max(a, b); }

    i32 max(i32 a, i32 b) { return SDL_max(a, b); }

    u8 max(u8 a, u8 b) { return SDL_max(a, b); }

    u16 max(u16 a, u16 b) { return SDL_max(a, b); }

    u32 max(u32 a, u32 b) { return SDL_max(a, b); }
}


/* round, ceil, floor */

namespace math
{
    f32 round(f32 num) { return SDL_roundf(num); }

    f64 round(f64 num) { return SDL_round(num); }

    f32 ceil(f32 num) { return SDL_ceilf(num); }

    f64 ceil(f64 num) { return SDL_ceil(num); }

    f32 floor(f32 num) { return SDL_floorf(num); }

    f64 floor(f64 num) { return SDL_floor(num); }
}


/* fma */

namespace math
{
    f32 fma(f32 a, f32 b, f32 c)
    {
    #ifdef MATH_SIMD_128
        return avx::fma(a, b, c);
    #else
        return a * b + c;
    #endif
    }


    f64 fma(f64 a, f64 b, f64 c)
    {
    #ifdef MATH_SIMD_128
        return avx::fma(a, b, c);
    #else
        return a * b + c;
    #endif
    }
}


/* fmod */

namespace math
{
    f32 fmod(f32 num, f32 den) { return SDL_fmodf(num, den); }

    f64 fmod(f64 num, f64 den) { return SDL_fmod(num, den); }

    f32 modf(f32 num, f32* iptr) { return SDL_modff(num, iptr); }

    f64 modf(f64 num, f64* iptr) { return SDL_modf(num, iptr); }
}


/* exp, pow, log */

namespace math
{
    f32 exp(f32 num) { return SDL_expf(num); }

    f64 exp(f64 num) { return SDL_exp(num); }

    f32 pow(f32 base, f32 exp) { return SDL_powf(base, exp); }

    f64 pow(f64 base, f64 exp) { return SDL_pow(base, exp); }

    f32 log(f32 num) { return SDL_logf(num); }

    f64 log(f64 num) { return SDL_log(num); }

    f32 log10(f32 num) { return SDL_log10f(num); }

    f64 log10(f64 num) { return SDL_log10(num); }

    
}


/* sqrt, rsqrt, hypot */

namespace math
{
    f32 sqrt(f32 num) { return SDL_sqrtf(num); }

    f64 sqrt(f64 num) { return SDL_sqrt(num); }


    f32 rsqrt(f32 num)
    {
    #ifdef MATH_SIMD_128
        return avx::rsqrt(num);
    #else
        return 1.0f / SDL_sqrtf(num);
    #endif
    }


    f64 rsqrt(f64 num)
    {
    #ifdef MATH_SIMD_128
        return avx::rsqrt(num);
    #else
        return 1.0 / SDL_sqrt(num);
    #endif
    }


    f32 hypot(f32 a, f32 b) { return sqrt(fma(a, a, b * b)); }

    f64 hypot(f64 a, f64 b) { return sqrt(fma(a, a, b * b)); }

    f32 rhypot(f32 a, f32 b) { return 1.0f / hypot(a, b); }

    f64 rhypot(f64 a, f64 b) { return 1.0 / hypot(a, b); }
}


/* trig */

namespace math
{
    f32 sin(f32 rad) { return SDL_sinf(rad); }

    f64 sin(f64 rad) { return SDL_sin(rad); }

    f32 cos(f32 rad) { return SDL_cosf(rad); }

    f64 cos(f64 rad) { return SDL_cos(rad); }

    f32 tan(f32 rad) { return SDL_tanf(rad); }

    f64 tan(f64 rad) { return SDL_tan(rad); }

    f32 asin(f32 sin) { return SDL_asinf(sin); }

    f64 asin(f64 sin) { return SDL_asin(sin); }

    f32 acos(f32 cos) { return SDL_acosf(cos); }

    f64 acos(f64 cos) { return SDL_acos(cos); }

    f32 atan(f32 tan) { return SDL_atanf(tan); }

    f64 atan(f64 tan) { return SDL_atan(tan); }

    f32 atan2(f32 sin, f32 cos) { return SDL_atan2f(sin, cos); }

    f64 atan2(f64 sin, f64 cos) { return SDL_atan2(sin, cos); }
}


/* random */

namespace math
{
    void rand_init()
    {
        SDL_srand(1);
    }


    f32 rand(f32 min, f32 max)
    {
        return min + SDL_randf() * (max - min);
    }
}