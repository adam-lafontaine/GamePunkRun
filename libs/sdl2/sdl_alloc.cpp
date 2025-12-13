#pragma once

#include "../alloc_type/alloc_type.hpp"
#include "sdl_include.hpp"


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
    static void* sdl_alloc(u32 n_elements, u32 element_size = 4)
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


    static void sdl_free(void* ptr)
    {
        SDL_free(ptr);
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

    
    void free_any(void* ptr)
    {
        alloc_type_log("free_any(%p)\n", ptr);
        SDL_free(ptr);
    }
}


#ifndef ALLOC_COUNT

namespace mem
{    
    void* alloc_memory(u32 n_elements, u32 element_size, cstr tag)
    {
        alloc_type_log("alloc_memory(%u, %u, %s)\n", n_elements, element_size, tag);

        return sdl_alloc(n_elements, element_size);
    }


    void add_memory(void* ptr, u32 n_elements, u32 element_size, cstr tag)
    {
        alloc_type_log("add_memory(%p, %u, %u, %s)\n", ptr, n_elements, element_size, tag);
    }


    void free_memory(void* ptr, u32 element_size)
    {
        alloc_type_log("free_memory(%p, %u)\n", ptr, element_size);
        sdl_free(ptr);
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

#include "../span/span.hpp"


namespace counts
{
    constexpr auto NO_TAG = "no tag";


    static constexpr cstr bit_width_str(u32 size)
    {
        switch (size)
        {
        case 1: return "8 bit";
        case 2: return "16 bit";
        case 4: return "32 bit";
        case 8: return "64 bit";
        case 16: return "128 bit";
        default: return "void/any";
        }
    }
    
    
    class AllocLog
    {
    public:
        static constexpr u32 capacity = mem::AllocationStatus::MAX_SLOTS;

        u32 size = 0;

        cstr tags[capacity] = {0};
        cstr actions[capacity] = {0};
        u32 sizes[capacity] = {0};
        u32 n_allocs[capacity] = {0};
        u32 n_bytes[capacity] = {0};
    };


    template <u32 ELE_SZ>
    class AllocCounts
    {
    public:
        static constexpr u32 element_size = ELE_SZ ? ELE_SZ : 1;
        static constexpr u32 max_allocations = mem::AllocationStatus::MAX_SLOTS;

        cstr type_name = bit_width_str(ELE_SZ);

        void* keys[max_allocations] = { 0 };
        u32 byte_counts[max_allocations] = { 0 };
        u32 element_counts[max_allocations] = { 0 };
        cstr tags[max_allocations] = { 0 };

        u32 bytes_allocated = 0;
        u32 elements_allocated = 0;
        u32 n_allocations = 0;

        AllocLog log;


        void update_element_counts(u32 slot)
        {
            elements_allocated = bytes_allocated / element_size;
            element_counts[slot] = byte_counts[slot] / element_size;
        }


        void log_alloc(cstr action, u32 slot, void* ptr)
        {
            auto i = log.size;
            log.size++;

            log.tags[i] = tags[slot];
            log.actions[i] = action;
            log.sizes[i] = byte_counts[slot];
            log.n_allocs[i] = n_allocations;
            log.n_bytes[i] = bytes_allocated;

            alloc_type_log("%s<%u>(%s) | %s(%p) | %u/%u (%u)\n", action, element_size, type_name, tags[slot], ptr, n_allocations, max_allocations, bytes_allocated);
        }


        void* add_allocation(u32 n_elements, cstr tag)
        {
            // find next available slot
            u32 i = 0; 
            for (;i < max_allocations && keys[i]; i++)
            { }

            if (i >= max_allocations || keys[i])
            {
                alloc_type_assert("*** Allocation limit reached ***" && false);
                alloc_type_log("Allocation limit reached (%u)\n", element_size);
                return 0;
            }

            auto data = mem::sdl_alloc(n_elements, element_size);
            if (!data)
            {
                alloc_type_assert("*** Allocation failed ***" && false);
                alloc_type_log("Allocation failed");
                return 0;
            }            

            u32 const n_bytes = n_elements * element_size;

            n_allocations++;
            bytes_allocated += n_bytes;
            keys[i] = data;
            byte_counts[i] = n_bytes;
            tags[i] = tag ? tag : NO_TAG;

            update_element_counts(i);
            log_alloc("alloc", i, data);

            return data;
        }


        void add_allocated(void* data, u32 n_elements, cstr tag)
        {
            // find next available slot
            u32 i = 0; 
            for (;i < max_allocations && keys[i]; i++)
            { }

            if (i >= max_allocations || keys[i])
            {
                alloc_type_assert("*** Allocation limit reached ***" && false);
                alloc_type_log("Allocation limit reached (%u)\n", element_size);
                return;
            }

            u32 const n_bytes = n_elements * element_size;

            n_allocations++;
            bytes_allocated += n_bytes;
            keys[i] = data;
            byte_counts[i] = n_bytes;
            tags[i] = tag ? tag : NO_TAG;

            update_element_counts(i);
            log_alloc("add", i, data);
        }


        bool remove_allocation(void* ptr)
        {
            // find slot
            u32 i = 0; 
            for (; i < max_allocations && keys[i] != ptr; i++)
            { }

            if (i >= max_allocations)
            {
                return false;
            }

            mem::sdl_free(keys[i]);
            n_allocations--;
            bytes_allocated -= byte_counts[i];
            keys[i] = 0;
            tags[i] = 0;
            byte_counts[i] = 0;

            update_element_counts(i);
            log_alloc("free", i, ptr);

            return true;
        }


        void tag_allocation(void* ptr, u32 n_elements, cstr tag)
        {
            // find slot, if any
            u32 i = 0; 
            for (;i < max_allocations && keys[i] != ptr; i++)
            { }

            if (i >= max_allocations)
            {
                // already tagged
                return;
            }

            // find next available slot
            i = 0; 
            for (;i < max_allocations && keys[i]; i++)
            { }

            if (i >= max_allocations || keys[i])
            {
                alloc_type_assert("*** Allocation limit reached ***" && false);
                alloc_type_log("Allocation limit reached (%u)\n", element_size);
                return;
            }

            u32 const n_bytes = n_elements * element_size;

            n_allocations++;
            bytes_allocated += n_bytes;
            keys[i] = ptr;
            byte_counts[i] = n_bytes;
            tags[i] = tag;

            update_element_counts(i);
            log_alloc("tagged", i, ptr);
        }


        void untag_allocation(void* ptr)
        {
            // find slot
            u32 i = 0; 
            for (; i < max_allocations && keys[i] != ptr; i++)
            { }

            if (i >= max_allocations)
            {
                alloc_type_assert("*** Allocation not found ***" && false);
                alloc_type_log("Allocation not found (%u)\n", element_size);
                return;
            }

            n_allocations--;
            bytes_allocated -= byte_counts[i];
            keys[i] = 0;
            tags[i] = 0;
            byte_counts[i] = 0;

            update_element_counts(i);
            log_alloc("untagged", i, ptr);
        }
    };
}


/* static data */

namespace mem
{
    counts::AllocCounts<1> alloc_8;
    counts::AllocCounts<2> alloc_16;
    counts::AllocCounts<4> alloc_32;
    counts::AllocCounts<8> alloc_64;
    counts::AllocCounts<16> alloc_128;
}


/* helpers */

namespace mem
{
    static void free_unknown(void* ptr)
    {
        auto free = 
            alloc_8.remove_allocation(ptr) ||
            alloc_16.remove_allocation(ptr) ||
            alloc_32.remove_allocation(ptr) ||
            alloc_64.remove_allocation(ptr) ||
            alloc_128.remove_allocation(ptr);

        if (free)
        {
            return;
        }
        
        //alloc_type_assert("*** Allocation not found ***" && false);

        alloc_type_log("Allocation not found (%p)\n", ptr);

        sdl_free(ptr);
    }


    static bool free_allocation(void* ptr, u32 element_size)
    {
        switch (element_size)
        {
        case 1: return alloc_8.remove_allocation(ptr);
        case 2: return alloc_16.remove_allocation(ptr);
        case 4: return alloc_32.remove_allocation(ptr);
        case 8: return alloc_64.remove_allocation(ptr);
        case 16: return alloc_128.remove_allocation(ptr);
        default: return alloc_8.remove_allocation(ptr);
        }
    }


    template <u32 ELE_SZ>
    static void set_status(counts::AllocCounts<ELE_SZ> const& src, AllocationStatus& dst)
    {
        dst.type_name = src.type_name;
        dst.element_size = src.element_size;
        dst.max_allocations = src.max_allocations;

        dst.bytes_allocated = src.bytes_allocated;
        dst.elements_allocated = src.elements_allocated;
        dst.n_allocations = src.n_allocations;

        u32 d = 0;
        for (u32 i = 0; i < src.max_allocations; i++)
        {
            if (src.keys[i])
            {
                dst.slot_tags[d] = src.tags[i];
                dst.slot_sizes[d] = src.byte_counts[i];
                d++;
            }            
        }
    }


    template <u32 ELE_SZ>
    static void set_history(counts::AllocCounts<ELE_SZ> const& src, AllocationHistory& dst)
    {
        dst.type_name = src.type_name;
        dst.element_size = src.element_size;
        dst.max_allocations = src.max_allocations;

        auto& log = src.log;

        dst.n_items = (u32)log.size;

        if (dst.n_items)
        {
            dst.tags = (cstr*)log.tags;
            dst.actions = (cstr*)log.actions;
            dst.sizes = (u32*)log.sizes;
            dst.n_allocs = (u32*)log.n_allocs;
            dst.n_bytes = (u32*)log.n_bytes;
        }
    }
}


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


/* ALLOC_COUNT */

namespace mem
{
    AllocationStatus query_status(u32 element_size)
    {
        AllocationStatus status{};

        switch (element_size)
        {
        case 1: set_status(alloc_8, status); break;
        case 2: set_status(alloc_16, status); break;
        case 4: set_status(alloc_32, status); break;
        case 8: set_status(alloc_64, status); break;
        case 16: set_status(alloc_128, status); break;        
        default: set_status(alloc_8, status); break;
        }

        return status;
    }


    AllocationHistory query_history(u32 element_size)
    {
        AllocationHistory history{};

        switch (element_size)
        {
        case 1: set_history(alloc_8, history); break;
        case 2: set_history(alloc_16, history); break;
        case 4: set_history(alloc_32, history); break;
        case 8: set_history(alloc_64, history); break;
        case 16: set_history(alloc_128, history); break;        
        default: set_history(alloc_8, history); break;
        }

        return history;
    }
}

#endif