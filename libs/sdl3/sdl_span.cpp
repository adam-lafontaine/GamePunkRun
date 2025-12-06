#pragma once

#include "../span/span.hpp"

#include <SDL3/SDL.h>


/* api */

namespace span
{
    void copy_u8(u8* src, u8* dst, u64 len_u8)
    {
        SDL_memcpy(dst, src, len_u8);
    }


    void copy_u8(u8* src, u8* dst1, u8* dst2, u64 len_u8)
    {
        SDL_memcpy(dst1, src, len_u8);
        SDL_memcpy(dst2, src, len_u8);
    }


    void fill_u8(u8* dst, u8 value, u64 len_u8)
    {
        SDL_memset(dst, value, len_u8);
    }


    void fill_u32(u32* dst, u32 value, u64 len_u32)
    {
        SDL_memset4(dst, value, len_u32);
    }
}