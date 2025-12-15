#pragma once

#include "../util/types.hpp"

// for testing
//#ifndef ALLOC_COUNT
//#define ALLOC_COUNT
//#endif




namespace mem
{
    void* alloc_any(u32 n_elements, u32 element_size);
    
    void free_any(void* ptr);
}


namespace mem
{    
    void* alloc_memory(u32 n_elements, u32 element_size, cstr tag);

    void add_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag);

    void free_memory(void* ptr, u32 element_size);

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


#ifdef ALLOC_COUNT

namespace mem
{ 

    struct AllocationStatus
    {
        cstr type_name = 0;
        u32 element_size = 0;
        u32 max_allocations = 0;

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
        u32 max_allocations = 0;

        u32 n_items = 0;

        cstr* tags = 0;
        cstr* actions = 0;
        u32* sizes = 0;
        u32* n_allocs = 0;
        u32* n_bytes = 0;

    };


    AllocationStatus query_status(u32 element_size);

    AllocationHistory query_history(u32 element_size);
}

#endif