#pragma once

#include "../util/types.hpp"

#include <cassert>


/* handles */

namespace mem
{
    class Handle8;
    class Handle16;
    class Handle32;
    class Handle64;

    class MemoryContext;


    template <typename T>
    class Span
    {
    public:
        T* data = 0;
        u32 length = 0;
    };
}


/*  */

namespace mem
{ 
    bool alloc(MemoryContext* ctx);

    void close(MemoryContext* ctx);

    Handle8* reserve_8(MemoryContext* ctx, u32 count);

    Handle16* reserve_16(MemoryContext* ctx, u32 count);

    Handle32* reserve_32(MemoryContext* ctx, u32 count);

    Handle64* reserve_64(MemoryContext* ctx, u32 count);

    Span<u8> get(Handle8* handle);

    Span<u16> get(Handle16* handle);

    Span<u32> get(Handle32* handle);

    Span<u64> get(Handle64* handle);


    template <typename T>
    
}