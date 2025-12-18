#pragma once

#include "alloc_type.hpp"
#include "../span/span.hpp"

#include <cstdlib>
#include <cassert>

//#define LOG_ALLOC_TYPE

#if !defined NDEBUG && defined LOG_ALLOC_TYPE

#include <cstdio>

#define alloc_type_log(...) printf(__VA_ARGS__)

#else
#define alloc_type_log(...)
#endif


namespace mem
{

    static void* unaligned_alloc(u32 n_elements, u32 element_size)
    {
        auto n_bytes = n_elements * element_size;
        return std::malloc(n_bytes);
    }


    static void unaligned_free(void* ptr)
    {
        std::free(ptr);
    }


    static void* aligned_alloc(u32 n_elements, u32 element_size)
    {
        alloc_type_log("alloc_memory(%u, %u, %s)\n", n_elements, element_size);

        auto n_bytes = n_elements * element_size;

    #if defined _WIN32 || defined __EMSCRIPTEN__

        return std::malloc(n_bytes);

    #else
        u32 alignment = 4;

        switch (element_size)
        {
        case 2:
        case 4:
        case 8:
        case alignof(std::max_align_t):
            alignment = element_size;            
            break;
        
        default:
            break;
        }

        auto data = std::aligned_alloc(alignment, n_bytes);

        //span::fill(span::make_view((u8*)data, n_bytes), (u8)0);        

        return data;

    #endif
    }


    static inline void aligned_free(void* ptr, u32 element_size)
    {
        unaligned_free(ptr);
    }
}


namespace mem
{
    void* alloc_any(u32 n_elements, u32 element_size)
    {
        alloc_type_log("alloc_any(%u, %u)\n", n_elements, element_size);

        auto n_bytes = n_elements * element_size;
        return std::malloc(n_bytes);
    }

    
    void free_any(void* ptr)
    {
        alloc_type_log("free_any(%p)\n", ptr);
        std::free(ptr);
    }


    void* alloc_memory(u32 n_elements, u32 element_size)
    {
        alloc_type_log("alloc_memory(%u, %u)\n", n_elements, element_size);
        return aligned_alloc(n_elements, element_size);
    }
}


#ifndef ALLOC_COUNT

namespace mem
{    
    void* alloc_memory(u32 n_elements, u32 element_size, cstr tag)
    {
        alloc_type_log("alloc_memory(%u, %u, %s)\n", n_elements, element_size, tag);

        return unaligned_alloc(n_elements, element_size);
    }


    void free_memory(void* ptr, u32 element_size)
    {
        alloc_type_log("free_memory(%p, %u)\n", ptr, element_size);
        unaligned_free(ptr);
    }


    void add_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag)
    {
        alloc_type_log("add_memory(%p, %u, %u, %s)\n", ptr, n_elements, element_size, tag);
    }


    void tag_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag)
    {
        alloc_type_log("tag_memory(%p, %u, %u, %s)\n", ptr, n_elements, element_size, tag);
    }


    void tag_file_memory(void* ptr, u32 element_size, cstr file_path)
    {
        alloc_type_log("tag_file_memory(%p, %u, %s)", ptr, element_size, file_path);
    }


    void untag_memory(void* ptr, u32 element_size)
    {
        alloc_type_log("untag_memory(%p, %u)\n", ptr, element_size);
    }
}

#else

#include "alloc_count.hpp"


/* api */

namespace mem
{
    void* alloc_memory(u32 n_elements, u32 element_size, cstr tag)
    {
        switch (element_size)
        {
        case 1: return alloc_8.add_allocation(n_elements, tag);
        case 2: return alloc_16.add_allocation(n_elements, tag);
        case 4: return alloc_32.add_allocation(n_elements, tag);
        case 8: return alloc_64.add_allocation(n_elements, tag);
        case 16: return alloc_128.add_allocation(n_elements, tag);
        default: return alloc_8.add_allocation(n_elements * element_size, tag);
        }
    }


    void add_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag)
    {
        switch (element_size)
        {
        case 2: alloc_16.add_allocated(ptr, n_elements, tag); break;
        case 4: alloc_32.add_allocated(ptr, n_elements, tag); break;
        case 8: alloc_64.add_allocated(ptr, n_elements, tag); break;
        case 16: alloc_128.add_allocated(ptr, n_elements, tag); break;
        default: alloc_8.add_allocated(ptr, n_elements, tag); break;
        }
    }


    void free_memory(void* ptr, u32 element_size)
    {
        if (!free_allocation(ptr, element_size))
        {
            alloc_type_log("free_unknown(%p): %u\n", ptr, element_size);
            free_unknown(ptr);
        }
    }


    void tag_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag)
    {
        switch (element_size)
        {
        case 1: alloc_8.tag_allocation(ptr, n_elements, tag); break;
        case 2: alloc_16.tag_allocation(ptr, n_elements, tag); break;
        case 4: alloc_32.tag_allocation(ptr, n_elements, tag); break;
        case 8: alloc_64.tag_allocation(ptr, n_elements, tag); break;
        case 16: alloc_128.tag_allocation(ptr, n_elements, tag); break;
        default: alloc_8.tag_allocation(ptr, n_elements, tag); break;
        }
    }


    void untag_memory(void* ptr, u32 element_size)
    {
        switch (element_size)
        {
        case 1: alloc_8.untag_allocation(ptr); break;
        case 2: alloc_16.untag_allocation(ptr); break;
        case 4: alloc_32.untag_allocation(ptr); break;
        case 8: alloc_64.untag_allocation(ptr); break;
        case 16: alloc_128.untag_allocation(ptr); break;
        default: alloc_8.untag_allocation(ptr); break;
        }
    }
}

#endif