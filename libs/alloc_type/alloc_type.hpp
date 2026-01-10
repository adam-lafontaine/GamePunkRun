#pragma once

#include "../util/types.hpp"

#define EDITING_ALLOC_COUNT

#ifdef EDITING_ALLOC_COUNT

#ifndef ALLOC_COUNT
#define ALLOC_COUNT
#endif

#endif




namespace mem
{    
    void* alloc_any(u32 n_elements, u32 element_size);
    
    void free_any(void* ptr);

    void* alloc_memory(u32 n_elements, u32 element_size);
}


namespace mem
{    
    void* alloc_memory(u32 n_elements, u32 element_size, cstr tag);

    void free_memory(void* ptr, u32 element_size);

    void add_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag);

    void tag_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag);

    void tag_file_memory(void* ptr, u32 element_size, cstr file_path);

    void untag_memory(void* ptr, u32 element_size);    
}


namespace mem
{
    template <typename T>
    inline T* alloc(u32 n_elements, cstr tag)
    {
        return (T*)alloc_memory(n_elements, (u32)sizeof(T), tag);
    }


    template <typename T>
    inline void add_allocation(T* data, u32 n_elements, cstr tag)
    {
        add_memory((void*)data, n_elements, (u32)sizeof(T), tag);
    }


    template <typename T>
    inline void free(T* ptr)
    {
        free_memory((void*)ptr, (u32)sizeof(T));
    }


    template <typename T>
    inline void tag(T* data, u32 n_elements, cstr tag)
    {
        tag_memory((void*)data, n_elements, (u32)sizeof(T), tag);
    }


    template <typename T>
    inline void untag(T* ptr)
    {
        untag_memory((void*)ptr, (u32)sizeof(T));
    }
}


namespace mem
{
    enum class Alloc : u32
    {
        Bytes_1 = 1,
        Bytes_2 = 2,
        Bytes_4 = 4,
        Bytes_8 = 8,

        STBI,
    };


    void* alloc_memory(u32 n_bytes, Alloc type);

    void* realloc_memory(void* ptr, u32 n_bytes, Alloc type);

    void free_memory(void* ptr, Alloc type);
}


/* special case stbi */

namespace mem
{
    inline void* alloc_stbi(u32 size)
    {
        return alloc_memory(size, Alloc::STBI);
    }


    inline void* realloc_stbi(void* ptr, u32 size)
    {
        return realloc_memory(ptr, size, Alloc::STBI);
    }


    void free_stbi(void* ptr)
    {
        free_memory(ptr, Alloc::STBI);
    }
}



#ifdef ALLOC_COUNT

namespace mem
{ 

    struct AllocationStatus
    {
        cstr type_name = 0;
        u32 element_size = 0;

        u32 bytes_allocated = 0;
        u32 elements_allocated = 0;

        u32 n_allocations = 0;
        
        cstr* slot_tags = 0;
        u32* slot_sizes = 0;
    };


    struct AllocationHistory
    {
        cstr type_name = 0;
        u32 element_size = 0;

        u32 n_items = 0;
        u32 max_bytes = 0;

        cstr* tags = 0;
        cstr* actions = 0;
        u32* sizes = 0;
        u32* n_allocs = 0;
        u32* n_bytes = 0;
    };


    AllocationStatus query_status(Alloc type);

    AllocationHistory query_history(Alloc type);
}

#endif