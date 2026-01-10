#pragma once

#include "../alloc_type/alloc_type.hpp"

#include <SDL2/SDL.h>


//#define LOG_ALLOC_TYPE
#define ASSERT_ALLOC_TYPE


#ifndef NDEBUG

#ifdef LOG_ALLOC_TYPE
#define alloc_type_log(...) SDL_Log(__VA_ARGS__)
#else
#define alloc_type_log(...)
#endif

#ifdef ASSERT_ALLOC_TYPE
#define alloc_type_assert(condition) SDL_assert(condition)
#else
#define alloc_type_assert(...)
#endif

#else

#define alloc_type_log(...)
#define alloc_type_assert(...)

#endif


namespace mem
{
    static void* unaligned_alloc(u32 n_elements, u32 element_size)
    {
        void* data = 0;
        auto n_bytes = n_elements * element_size;

        data = SDL_malloc(n_bytes);

        if (data)
        {
            SDL_memset(data, 0, n_bytes);
        }

        return data;
    }


    static void unaligned_free(void* ptr)
    {
        SDL_free(ptr);
    }


    // for counts
    static void* aligned_alloc(u32 n_elements, u32 element_size)
    {
        return unaligned_alloc(n_elements, element_size);
    }


    // for counts
    static void aligned_free(void* ptr, u32 element_size)
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
        return SDL_malloc(n_bytes);
    }


    void* realloc_any(void* ptr, u32 n_elements, u32 element_size)
    {
        alloc_type_log("realloc_any(%u, %u)\n", n_elements, element_size);

        auto n_bytes = n_elements * element_size;
        return SDL_realloc(ptr, n_bytes);
    }

    
    void free_any(void* ptr)
    {
        alloc_type_log("free_any(%p)\n", ptr);
        SDL_free(ptr);
    }


    void* alloc_memory(u32 n_elements, u32 element_size)
    {
        alloc_type_log("alloc_memory(%u, %u)\n", n_elements, element_size);
        return unaligned_alloc(n_elements, element_size);
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


namespace mem
{
    void* alloc_memory(u32 n_bytes, Alloc type)
    {
        switch (type)
        {
        case mem::Alloc::Bytes_1: return unaligned_alloc(n_bytes, 1u);
        case mem::Alloc::Bytes_2: return unaligned_alloc(n_bytes / 2, 2u);
        case mem::Alloc::Bytes_4: return unaligned_alloc(n_bytes / 4, 4u);
        case mem::Alloc::Bytes_8: return unaligned_alloc(n_bytes / 8, 8u);
        case Alloc::STBI: return alloc_any(n_bytes, 1u);

        default: return alloc_any(n_bytes, 1u);
        }            
    }


    void* realloc_memory(void* ptr, u32 n_bytes, Alloc type)
    {
        switch (type)
        {
        case Alloc::STBI: return realloc_any(ptr, n_bytes, 1u);

        default: return realloc_any(ptr, n_bytes, 1u);
        }            
    }


    void free_memory(void* ptr, Alloc type)
    {
        switch (type)
        {
        case mem::Alloc::Bytes_1:
        case mem::Alloc::Bytes_2:
        case mem::Alloc::Bytes_4:
        case mem::Alloc::Bytes_8:
            unaligned_free(ptr);
            break;

        case Alloc::STBI: 
            free_any(ptr);
            break;

        default: 
            free_any(ptr);
            break;
        }            
    }
}


#else

#include "../alloc_type/alloc_count.hpp"


/* api */

namespace mem
{
    void* alloc_memory(u32 n_elements, u32 element_size, cstr tag)
    {
        return counts::add_allocation(n_elements, element_size, tag);
    }


    void add_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag)
    {
        counts::add_allocated(ptr, n_elements, element_size, tag);
    }


    void free_memory(void* ptr, u32 element_size)
    {
        if (!counts::free_allocation(ptr, element_size))
        {
            alloc_type_log("free_unknown(%p): %u\n", ptr, element_size);
            counts::free_unknown(ptr);
        }
    }


    void tag_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag)
    {
        counts::tag_allocation(ptr, n_elements, element_size, tag);
    }


    void untag_memory(void* ptr, u32 element_size)
    {
        counts::untag_allocation(ptr, element_size);
    }
}


namespace mem
{
    void* alloc_memory(u32 n_bytes, Alloc type)
    {
        return counts::add_allocation(n_bytes, type);
    }


    void* realloc_memory(void* ptr, u32 n_bytes, Alloc type)
    {
        counts::free_allocation(ptr, type);
        return counts::add_allocation(n_bytes, type);
    }


    void free_memory(void* ptr, Alloc type)
    {
        counts::free_allocation(ptr, type);
    }
}

#endif