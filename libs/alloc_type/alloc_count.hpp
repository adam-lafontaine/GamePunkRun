#pragma once

#include "alloc_type.hpp"

#ifndef LOG_ALLOC_TYPE
#define alloc_type_log(...)
#endif

#ifndef ASSERT_ALLOC_TYPE
#define alloc_type_assert(...)
#endif


#ifdef ALLOC_COUNT

#include "../span/span.hpp"


namespace counts
{
    constexpr auto NO_TAG = "no tag";

    constexpr u32 MAX_SLOTS = 50;


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
        static constexpr u32 capacity = MAX_SLOTS;

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
        static constexpr u32 max_allocations = MAX_SLOTS;

        cstr type_name = bit_width_str(ELE_SZ);

        void* keys[max_allocations] = { 0 };
        u32 byte_counts[max_allocations] = { 0 };
        u32 element_counts[max_allocations] = { 0 };
        cstr tags[max_allocations] = { 0 };

        u32 bytes_allocated = 0;
        u32 elements_allocated = 0;
        u32 n_allocations = 0;

        AllocLog log;



    private:

        i32 find_available_slot()
        {
            i32 i = 0; 
            for (;i < max_allocations && keys[i]; i++)
            { }

            if (i >= max_allocations || keys[i])
            {
                alloc_type_assert("*** Allocation limit reached ***" && false);
                alloc_type_log("Allocation limit reached (%u)\n", element_size);
                return -1;
            }

            return i;
        }


        i32 find_slot(void* ptr)
        {
            // find slot
            u32 i = 0; 
            for (; i < max_allocations && keys[i] != ptr; i++)
            { }

            if (i >= max_allocations)
            {
                return -1;
            }

            return i;
        }

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
        

    public:

        void* add_allocation(u32 n_elements, cstr tag)
        {
            auto i = find_available_slot();
            if (i < 0)
            {
                return 0;
            }

            auto data = mem::aligned_alloc(n_elements, element_size);
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
            auto i = find_available_slot();
            if (i < 0)
            {
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
            auto i = find_slot(ptr);

            if (i < 0)
            {
                return false;
            }

            mem::aligned_free(keys[i], element_size);
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
            auto i = find_slot(ptr);
            if (i >= 0)
            {
                // already tagged
                return;
            }

            i = find_available_slot();
            if (i < 0)
            {
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
            auto i = find_slot(ptr);
            if (i < 0)
            {
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
        
        alloc_type_assert("*** Allocation not found ***" && false);

        alloc_type_log("Allocation not found (%p)\n", ptr);

        mem::unaligned_free(ptr);
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

#endif