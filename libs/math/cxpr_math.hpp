#pragma once

#include "../util/types.hpp"

/* types */

namespace math
{
    static constexpr f64 PI = 3.14159265358979323846;


    template <typename T>
    struct MinMax
    {
        T min;
        T max;
    };
}


/* min, max, clamp */

namespace math
{
namespace cxpr
{
    template <typename T>
    inline constexpr T min(T a, T b)
    {
        return a < b ? a : b;
    }


    template <typename T>
    inline constexpr T min(T a, T b, T c)
    {
        return min(min(a, b), c);
    }


    template <typename T>
    inline constexpr T max(T a, T b)
    {
        return a > b ? a : b;
    }


    template <typename T>
    inline constexpr T max(T a, T b, T c)
    {
        return max(max(a, b), c);
    }


    template <typename T>
    inline constexpr MinMax<T> minmax(T a, T b)
    {
        MinMax<T> mm{};

        mm.min = min(a, b);
        mm.max = max(a, b);

        return mm;
    }


    template <typename T>
    inline constexpr MinMax<T> minmax(T a, T b, T c)
    {
        MinMax<T> mm{};

        mm.min = min(a, b, c);
        mm.max = max(a, b, c);

        return mm;
    }


    template <typename T>
    inline constexpr T clamp(T value, T min, T max)
    {
        const T t = value < min ? min : value;
        return t > max ? max : t;
    }


    template <typename T, typename U>
    inline constexpr T clamp(T value, U min, U max)
    {
        const T t = value < min ? (T)min : value;
        return t > max ? (T)max : t;
    }


    template <typename T, typename U>
    inline constexpr T clamp(T value, MinMax<U> minmax)
    {
        auto min = minmax.min;
        auto max = minmax.max;

        const T t = value < min ? (T)min : value;
        return t > max ? (T)max : t;
    }
    
}
}


/* sign, round, abs */

namespace math
{
namespace cxpr
{
    template <typename T>
    inline constexpr bool is_unsigned()
    {
        return (T)((T)0 - (T)1) > (T)0;
    }


    template <typename R, typename T>
    inline constexpr R sign(T value)
    {
        static_assert(!is_unsigned<T>());

        constexpr R P = (R)1;
        constexpr R N = (R)-1;
        constexpr R Z = (R)0;

        return value ? ((f32)value < 0.0f ? N : P) : Z;
    }


    template <typename T>
    inline constexpr f32 sign_f32(T value)
    {           
        return sign<f32, T>(value);
    }


    template <typename T>
    inline constexpr i8 sign_i8(T value)
    {
        return sign<i8, T>(value);
    }


    template <typename T>
    inline constexpr T round_to_unsigned(f32 value)
    {
        static_assert(is_unsigned<T>());

        return (T)(value + 0.5f);
    }


    template <typename T>
    inline constexpr T round_to_signed(f32 value)
    {
        static_assert(!is_unsigned<T>());

        return (T)(value + sign<f32, f32>(value) * 0.5f);
    }


    template <typename T>
    inline constexpr T round_to_signed(f64 value)
    {
        static_assert(!is_unsigned<T>());

        return (T)(value + sign<f64, f64>(value) * 0.5);
    }


    template <typename T>
    inline constexpr T abs(T value)
    {
        return sign<T, T>(value) * value;
    }
}
}


/* round, ceil, floor */

namespace math
{
namespace cxpr
{
    inline constexpr f32 pow(f32 base, u32 exp)
    {
        f32 val = 1.0f;
        for (u32 i = 0; i < exp; i++)
        {
            val *= base;
        }

        return val;
    }


    template <u32 N>
    inline constexpr f32 round(f32 value)
    {
        constexpr auto f = pow(10.0f, N);
        constexpr auto i_f = 1.0f / f;

        return round_to_signed<i32>(value * f) * i_f;
    }


    template <typename T>
    inline constexpr T floor(T value)
    { 
        constexpr T zero = (T)0;

        auto i = (i64)value;
        if (value < zero && (T)i != value)
        {
            i -= 1;
        }

        return (T)i;
    }


    template <typename T>
    inline constexpr T ceil(T value)
    { 
        auto f = floor(value);
        return (T)(f + (f != value));
    }
}
}


/* misc */

namespace math
{
namespace cxpr
{
    template <typename uT>
    inline constexpr uT unsigned_max()
    {
        static_assert(is_unsigned<uT>());

        return (uT)((uT)0 - (uT)1);
    }


    template <typename uT>
    inline constexpr uT scale_to_unsigned(f32 value)
    {
        static_assert(is_unsigned<uT>());

        constexpr f32 max = (f32)unsigned_max<uT>() + 1.0f;

        auto s = sign_f32(value);

        value -= s * (u64)value;
        value = s > 0.0f ? value : (1.0f - value);

        return round_to_unsigned<uT>(max * value);
    }


    template <typename uSrc, typename uDst>
    inline constexpr uDst scale_unsigned(uSrc value)
    {
        static_assert(is_unsigned<uSrc>());
        static_assert(is_unsigned<uDst>());

        constexpr f32 i_smax = 1.0f / ((f32)unsigned_max<uSrc>() + 1.0f);
        constexpr f32 dmax = (f32)unsigned_max<uDst>() + 1.0f;

        return round_to_unsigned<uDst>(dmax * value * i_smax);
    }


    template <typename T>
    inline constexpr T inc_wrap(T value, T min, T max)
    {
        ++value;
        return value < min || value > max ? min : value;
    }


    template <typename SRC, typename DST>
    inline constexpr DST lerp(SRC val, MinMax<SRC> mm_src, MinMax<DST> mm_dst)
    {
        f64 D = mm_dst.max - mm_dst.min;
        f64 S = mm_src.max - mm_src.min;
        f64 s = val - mm_src.min;        

        return mm_dst.min + (DST)(D * s / S);
    }


    inline constexpr bool is_power_of_2(u64 num)
    {
        return (num && !(num & (num - 1)));
    }


    inline constexpr bool is_even(u64 num)
    {
        return num % 2 == 0;
    }
}
}


/* trig */

namespace math
{
namespace cxpr
{

namespace approx
{
    template <typename T>
    inline constexpr T sin_approx(T rad)
    {
        // best for small angles e.g. 0 - 45deg

        constexpr T B = (T)(4.0) / (T)(PI);
        constexpr T C = (T)(-4.0) / ((T)(PI * PI));
        constexpr T P = (T)(0.225);

        T y = B * rad + C * rad * abs(rad);
        y = P * (y * abs(y) - y) + y;

        return y;
    }


    template <typename T>
    inline constexpr T cos_approx(T rad)
    {
        // best for small angles e.g. 0 - 45deg

        constexpr T tp = (T)(1.0) / (T)(2 * PI);

        T x = rad * tp;

        x -= (T)(0.25) + floor(x + T(0.25));
        x *= (T)(16.0) * (abs(x) - (T)(0.5));
        x += (T)(0.225) * x * (abs(x) - (T)(1.0));

        return x;
    }


    inline constexpr f32 atan_approx(f32 tan)
    {
        f32 sq = tan * tan;

        constexpr f32 a1  =  0.99997726f;
        constexpr f32 a3  = -0.33262347f;
        constexpr f32 a5  =  0.19354346f;
        constexpr f32 a7  = -0.11643287f;
        constexpr f32 a9  =  0.05265332f;
        constexpr f32 a11 = -0.01172120f;        

        return tan * (a1 + sq * (a3 + sq * (a5 + sq * (a7 + sq * (a9 + sq * a11)))));
    }

}


    template <typename T>
    inline constexpr T deg_to_rad(T deg)
    {
        constexpr auto scale = PI / 180.0;
        return (T)(deg * scale);
    }


    template <typename T>
    inline constexpr T rad_to_deg(T rad)
    {
        constexpr auto scale = 180.0 / PI;
        return (T)(rad * scale);
    }


    template <typename uT>
    inline constexpr f32 uangle_max_f32()
    {
        static_assert(is_unsigned<uT>());

        return (f32)unsigned_max<uT>() + 1.0f;
    }


    template <typename uT>
    inline constexpr u64 uangle_max_u64()
    {
        static_assert(is_unsigned<uT>());

        return (u64)unsigned_max<uT>() + 1u;
    }


    template <typename uT>
    inline constexpr f32 unsigned_to_rad(uT a)
    {
        static_assert(is_unsigned<uT>());

        constexpr f32 max = uangle_max_f32<uT>();
        constexpr auto scale = 2 * PI / max;

        return (f32)(a * scale);
    }


    template <typename uT>
    inline constexpr uT rad_to_unsigned(f32 rad)
    {
        static_assert(is_unsigned<uT>());

        constexpr f32 TP = (f32)(2 * PI);
        constexpr f32 max = uangle_max_f32<uT>();

        auto n = rad / TP;
        auto f = n - (i64)n;
        f = f < 0.0f ? f + 1.0f : f;

        return round_to_unsigned<uT>(max * f);
    }
    

    inline constexpr f32 sin(uangle a)
    {
        static_assert(sizeof(uangle) <= sizeof(u32));

        constexpr f32 P = (f32)PI;
        constexpr f32 TP = (f32)(2 * PI);
        constexpr f32 HP = (f32)(PI / 2);

        // split full rotation into 8 x 45deg sections
        constexpr u64 max = uangle_max_u64<uangle>();
        constexpr u64 oct = max / 8;
        
        auto rad = unsigned_to_rad(a);

        switch (a / oct)
        {
            case 0: return approx::sin_approx(rad);
            case 1: return approx::cos_approx(HP - rad);
            case 2: return approx::cos_approx(rad - HP);
            case 3: return approx::sin_approx(P - rad);
            case 4: return -approx::sin_approx(rad - P);
            case 5: return -approx::cos_approx(P + HP - rad);
            case 6: return -approx::cos_approx(rad - (P + HP));
            case 7: return -approx::sin_approx(TP - rad);
            default: return 0.0f;
        }
    }


    inline constexpr f32 cos(uangle a)
    {
        static_assert(sizeof(uangle) <= sizeof(u32));

        constexpr f32 P = (f32)PI;
        constexpr f32 TP = (f32)(2 * PI);
        constexpr f32 HP = (f32)(PI / 2);

        // split full rotation into 8 x 45deg sections
        constexpr u64 max = uangle_max_u64<uangle>();
        constexpr u64 oct = max / 8;
        
        auto rad = unsigned_to_rad(a);

        switch (a / oct)
        {
            case 0: return approx::cos_approx(rad);
            case 1: return approx::sin_approx(HP - rad);
            case 2: return -approx::sin_approx(rad - HP);
            case 3: return -approx::cos_approx(P - rad);
            case 4: return -approx::cos_approx(rad - P);
            case 5: return -approx::sin_approx(P + HP - rad);
            case 6: return approx::sin_approx(rad - (P + HP));
            case 7: return approx::cos_approx(TP - rad);
            default: return 0.0f;
        }
    }


    inline constexpr f32 sin(f32 rad)
    {
        return sin(rad_to_unsigned<uangle>(rad));
    }


    inline constexpr f32 cos(f32 rad)
    {
        return cos(rad_to_unsigned<uangle>(rad));
    }


    inline constexpr f32 atan2(f32 sin, f32 cos)
    {
        constexpr f32 P = (f32)PI;
        constexpr f32 TP = (f32)(2 * PI);
        constexpr f32 HP = (f32)(PI / 2);
        constexpr f32 QP = (f32)(PI / 4);

        assert(abs((cos * cos + sin * sin) - 1.0f) < 0.001f);

        auto pcos = abs(cos);
        auto psin = abs(sin);

        auto flip_45 = pcos < psin;
        auto flip_y = cos < 0.0f;
        auto flip_x = sin < 0.0f;

        auto key_y = (int)flip_y << 2;
        auto key_x = (int)flip_x << 1;
        auto key_45 = (int)flip_45;

        int oct_key = key_y | key_x | key_45;

        auto tan = flip_45 ? pcos / psin : psin / pcos;
        auto rad = approx::atan_approx(tan);
        
        assert(rad >= 0.0f);
        assert(rad <= QP);

        switch (oct_key)
        {
        case 0b000:
            // octant 0
            break;

        case 0b001:
            rad = HP - rad; // octant 1
            break;

        case 0b101:
            rad = HP + rad; // octant 2
            break;

        case 0b100:
            rad = P - rad; // octant 3
            break;

        case 0b110:
            rad = P + rad; // octant 4
            break;

        case 0b111:
            rad = 3 * HP - rad; // octant 5
            break;

        case 0b011:
            rad = 3 * HP + rad; // octant 6
            break;

        case 0b010:
            rad = TP - rad; // octant 7
            break;
        
        default:
            break;
        }

        return rad;
    }
    
    
    inline constexpr uangle atan2_u(f32 sin, f32 cos)
    { 
        auto rad = atan2_u(sin, cos);

        return rad_to_unsigned<uangle>(rad);
    }
}
}


/*  */

namespace math
{
namespace cxpr
{
    inline constexpr f32 modf(f32 num, f32* iptr)
    {
        auto i = (i32)num;
        *iptr = (f32)i;

        return num - i;
    }


    inline constexpr f64 modf(f64 num, f64* iptr)
    {
        auto i = (i64)num;
        *iptr = (f64)i;

        return num - i;
    }
}
}


