#include "image.hpp"


namespace image
{
namespace convert
{
    constexpr Pixel hsv_to_rgba(f32 h, f32 s, f32 v)
    {
        auto max = v;
        auto range = s * v;
        auto min = max - range;

        auto d = h * 6.0f; // 360.0f / 60.0f;

        auto h_id = (int)d;
        auto ratio = d - h_id;

        auto rise = min + ratio * range;
        auto fall = max - ratio * range;

        auto f32_to_u8 = [](f32 v){ return (u8)(v * 255 + 0.5f); };
        
        f32 rf = 0.0f;
        f32 gf = 0.0f;
        f32 bf = 0.0f;

        switch (h_id)
        {
        case 0:
            rf = max;
            gf = rise;
            bf = min;
            break;
        case 1:
            rf = fall;
            gf = max;
            bf = min;
            break;
        case 2:
            rf = min;
            gf = max;
            bf = rise;
            break;
        case 3:
            rf = min;
            gf = fall;
            bf = max;
            break;
        case 4:
            rf = rise;
            gf = min;
            bf = max;
            break;
        default:
            rf = max;
            gf = min;
            bf = fall;
            break;
        }

        u8 r = f32_to_u8(rf);
        u8 g = f32_to_u8(gf);
        u8 b = f32_to_u8(bf);

        return to_pixel(r, g, b);
    }


    constexpr Pixel hsv_to_rgba(u8 h, u8 s, u8 v)
    {
        f32 hf = h / 255.0f;
        f32 sf = s / 255.0f;
        f32 vf = v / 255.0f;

        return hsv_to_rgba(hf, sf, vf);
    }
}
}