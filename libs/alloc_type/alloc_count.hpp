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

#include <vector>


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

        u32 size = 0;

        std::vector<cstr> tags;
        std::vector<cstr> actions;
        std::vector<u32> sizes;
        std::vector<u32> n_allocs;
        std::vector<u32> n_bytes;

        AllocLog()
        {
            auto N = 100;
            tags.reserve(N);
            actions.reserve(N);
            sizes.reserve(N);
            n_allocs.reserve(N);
            n_bytes.reserve(N);
        }
    };
    

    class AllocCounts
    {
    public:
        
        static constexpr u32 max_allocations = MAX_SLOTS;

        u32 element_size = 0;

        cstr type_name = bit_width_str(0);

        void* keys[max_allocations] = { 0 };
        u32 byte_counts[max_allocations] = { 0 };
        u32 element_counts[max_allocations] = { 0 };
        cstr tags[max_allocations] = { 0 };

        u32 bytes_allocated = 0;
        u32 elements_allocated = 0;
        u32 n_allocations = 0;

        AllocLog log;

        AllocCounts() = delete;

        AllocCounts(u32 ele_sz) 
        {
            element_size = ele_sz; 
            alloc_type_assert(element_size);

            type_name = bit_width_str(element_size);
        }

        AllocCounts(u32 ele_sz, cstr name) 
        {
            element_size = ele_sz; 
            alloc_type_assert(element_size);

            type_name = name;
        }

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

            log.tags.push_back(tags[slot]);
            log.actions.push_back(action);
            log.sizes.push_back(byte_counts[slot]);
            log.n_allocs.push_back(n_allocations);
            log.n_bytes.push_back(bytes_allocated);

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
            assert(tag && "*** No tag set ***");

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
    counts::AllocCounts alloc_counts_8(1);
    counts::AllocCounts alloc_counts_16(2);
    counts::AllocCounts alloc_counts_32(4);
    counts::AllocCounts alloc_counts_64(8);
    //counts::AllocCounts alloc_128(16);

    counts::AllocCounts alloc_counts_stbi(1, "stbi");

    cstr status_slot_tags[counts::MAX_SLOTS] = { 0 };
    u32 status_slot_sizes[counts::MAX_SLOTS] = { 0 };

    cstr history_tags[counts::MAX_SLOTS] = { 0 };
    cstr history_actions[counts::MAX_SLOTS] = { 0 };
    u32 history_sizes[counts::MAX_SLOTS] = { 0 };
    u32 history_n_allocs[counts::MAX_SLOTS] = { 0 };
    u32 history_n_bytes[counts::MAX_SLOTS] = { 0 };
}


/* helpers */

namespace mem
{
    static void free_unknown(void* ptr)
    {
        auto free = 
            alloc_counts_8.remove_allocation(ptr) ||
            alloc_counts_16.remove_allocation(ptr) ||
            alloc_counts_32.remove_allocation(ptr) ||
            alloc_counts_64.remove_allocation(ptr);

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
        case 1: return alloc_counts_8.remove_allocation(ptr);
        case 2: return alloc_counts_16.remove_allocation(ptr);
        case 4: return alloc_counts_32.remove_allocation(ptr);
        case 8: return alloc_counts_64.remove_allocation(ptr);
        default: return alloc_counts_8.remove_allocation(ptr);
        }
    }
    

    static void set_status(counts::AllocCounts const& src, AllocationStatus& dst)
    {
        dst.type_name = src.type_name;
        dst.element_size = src.element_size;
        dst.max_allocations = src.max_allocations;

        dst.bytes_allocated = src.bytes_allocated;
        dst.elements_allocated = src.elements_allocated;
        dst.n_allocations = src.n_allocations;

        u32 d = 0;
        for (u32 i = 0; i < src.n_allocations; i++)
        {
            if (src.keys[i])
            {
                dst.slot_tags[d] = src.tags[i];
                dst.slot_sizes[d] = src.byte_counts[i];
                d++;
            }            
        }
    }
    

    static void set_history(counts::AllocCounts const& src, AllocationHistory& dst)
    {
        dst.type_name = src.type_name;
        dst.element_size = src.element_size;
        dst.max_allocations = src.max_allocations;

        auto& log = src.log;

        dst.n_items = (u32)log.size;

        if (dst.n_items)
        {
            dst.tags = (cstr*)log.tags.data();
            dst.actions = (cstr*)log.actions.data();
            dst.sizes = (u32*)log.sizes.data();
            dst.n_allocs = (u32*)log.n_allocs.data();
            dst.n_bytes = (u32*)log.n_bytes.data();
        }
    }
}


namespace mem
{
    AllocationStatus query_status(Alloc type)
    {
        AllocationStatus status{};
        status.slot_tags = status_slot_tags;
        status.slot_sizes = status_slot_sizes;

        switch (type)
        {
        case Alloc::Bytes_1: set_status(alloc_counts_8, status); break;
        case Alloc::Bytes_2: set_status(alloc_counts_16, status); break;
        case Alloc::Bytes_4: set_status(alloc_counts_32, status); break;
        case Alloc::Bytes_8: set_status(alloc_counts_64, status); break;
        default: set_status(alloc_counts_8, status); break;
        }

        return status;
    }


    AllocationHistory query_history(Alloc type)
    {
        AllocationHistory history{};
        history.tags = history_tags;
        history.actions = history_actions;
        history.sizes = history_sizes;
        history.n_allocs = history_n_allocs;
        history.n_bytes = history_n_bytes;

        switch (type)
        {
        case Alloc::Bytes_1: set_history(alloc_counts_8, history); break;
        case Alloc::Bytes_2: set_history(alloc_counts_16, history); break;
        case Alloc::Bytes_4: set_history(alloc_counts_32, history); break;
        case Alloc::Bytes_8: set_history(alloc_counts_64, history); break;
        default: set_history(alloc_counts_8, history); break;
        }

        return history;
    }
}

#endif