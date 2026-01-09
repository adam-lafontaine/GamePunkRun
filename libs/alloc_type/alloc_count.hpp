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
#include <unordered_map>


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


    class AllocationRecord
    {
    public:
        cstr tag = 0;
        u32 n_bytes = 0;
    };


    using AllocationList = std::unordered_map<u64, AllocationRecord>;
    
    
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

}



/* alloc counts */

namespace counts
{ 
    class AllocCounts
    {
    public:

        u32 element_size = 0;

        cstr type_name = bit_width_str(0);

        AllocationList list;

        u32 bytes_allocated = 0;
        u32 elements_allocated = 0;
        u32 n_allocations = 0;

        u32 max_bytes = 0;

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

        void update_element_counts()
        {
            elements_allocated = bytes_allocated / element_size;
            if (bytes_allocated > max_bytes)
            {
                max_bytes = bytes_allocated;
            }
        }


        void log_alloc(cstr action, void* ptr)
        {
            auto& record = list[(u64)ptr];
            
            log.size++;

            log.actions.push_back(action);
            log.tags.push_back(record.tag);            
            log.sizes.push_back(record.n_bytes);
            log.n_allocs.push_back(n_allocations);
            log.n_bytes.push_back(bytes_allocated);

            alloc_type_log("%s<%u>(%s) | %s(%p) | %u/%u (%u)\n", action, element_size, type_name, tags[slot], ptr, n_allocations, max_allocations, bytes_allocated);
        }
        

    public:

        void* add_allocation(u32 n_elements, cstr tag)
        {
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
            list[(u64)data] = { (tag ? tag : NO_TAG), n_bytes };

            update_element_counts();
            log_alloc("alloc", data);

            return data;
        }


        void add_allocated(void* data, u32 n_elements, cstr tag)
        { 
            u32 const n_bytes = n_elements * element_size;

            n_allocations++;
            bytes_allocated += n_bytes;
            list[(u64)data] = { (tag ? tag : NO_TAG), n_bytes };

            update_element_counts();
            log_alloc("add", data);
        }


        bool remove_allocation(void* ptr)
        {
            auto key = (u64)ptr;
            if (!list.contains(key))
            {
                return false;
            }

            auto record = list[key];

            mem::aligned_free(ptr, element_size);
            
            n_allocations--;
            bytes_allocated -= record.n_bytes;            

            update_element_counts();
            log_alloc("free", ptr);

            list.erase(key);

            return true;
        }


        void tag_allocation(void* ptr, u32 n_elements, cstr tag) // delete?
        {
            assert(tag && "*** No tag set ***");

            auto key = (u64)ptr;
            if (list.contains(key))
            {
                // already tagged
                return;
            }

            u32 const n_bytes = n_elements * element_size;

            n_allocations++;
            bytes_allocated += n_bytes;
            
            list[key] = { tag, n_bytes };

            update_element_counts();
            log_alloc("tagged", ptr);
        }


        void untag_allocation(void* ptr) // delete?
        {
            auto key = (u64)ptr;
            if (!list.contains(key))
            {
                return;
            }

            auto record = list[key];

            n_allocations--;
            bytes_allocated -= record.n_bytes;
            list.erase(key);

            update_element_counts();
            log_alloc("untagged", ptr);
        }
    };
}


/* alloc stbi counts */

namespace counts
{
    class AllocSTBICounts
    {
    public:
        
    };
}

/* static data */

namespace counts
{
    AllocCounts alloc_counts_8(1);
    AllocCounts alloc_counts_16(2);
    AllocCounts alloc_counts_32(4);
    AllocCounts alloc_counts_64(8);

    AllocCounts alloc_counts_stbi(1, "stbi");

    std::vector<cstr> status_tags;
    std::vector<u32> status_sizes;
}


/* helpers */

namespace counts
{
    inline void* add_allocation(u32 n_elements, u32 element_size, cstr tag)
    {
        switch (element_size)
        {
        case 1: return alloc_counts_8.add_allocation(n_elements, tag);
        case 2: return alloc_counts_16.add_allocation(n_elements, tag);
        case 4: return alloc_counts_32.add_allocation(n_elements, tag);
        case 8: return alloc_counts_64.add_allocation(n_elements, tag);
        default: return alloc_counts_8.add_allocation(n_elements * element_size, tag);
        }
    }


    inline void add_allocated(void* ptr, u32 n_elements, u32 element_size, cstr tag)
    {
        switch (element_size)
        {
        case 2: alloc_counts_16.add_allocated(ptr, n_elements, tag); break;
        case 4: alloc_counts_32.add_allocated(ptr, n_elements, tag); break;
        case 8: alloc_counts_64.add_allocated(ptr, n_elements, tag); break;
        default: alloc_counts_8.add_allocated(ptr, n_elements, tag); break;
        }
    }
    
    
    inline void free_unknown(void* ptr)
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


    inline bool free_allocation(void* ptr, u32 element_size)
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


    inline void tag_allocation(void* ptr, u32 n_elements, u32 element_size, cstr tag)
    {
        switch (element_size)
        {
        case 1: alloc_counts_8.tag_allocation(ptr, n_elements, tag); break;
        case 2: alloc_counts_16.tag_allocation(ptr, n_elements, tag); break;
        case 4: alloc_counts_32.tag_allocation(ptr, n_elements, tag); break;
        case 8: alloc_counts_64.tag_allocation(ptr, n_elements, tag); break;
        default: alloc_counts_8.tag_allocation(ptr, n_elements, tag); break;
        }
    }


    inline void untag_allocation(void* ptr, u32 element_size)
    {
        switch (element_size)
        {
        case 1: alloc_counts_8.untag_allocation(ptr); break;
        case 2: alloc_counts_16.untag_allocation(ptr); break;
        case 4: alloc_counts_32.untag_allocation(ptr); break;
        case 8: alloc_counts_64.untag_allocation(ptr); break;
        default: alloc_counts_8.untag_allocation(ptr); break;
        }
    }


    inline void* add_allocation(u32 n_bytes, mem::Alloc type)
    {
        constexpr auto tag = "mem::Alloc";
        constexpr auto stbi_tag = "stbi";

        switch (type)
        {
        case mem::Alloc::Bytes_1: return alloc_counts_8.add_allocation(n_bytes, tag);
        case mem::Alloc::Bytes_2: return alloc_counts_16.add_allocation(n_bytes / 2, tag);
        case mem::Alloc::Bytes_4: return alloc_counts_32.add_allocation(n_bytes / 4, tag);
        case mem::Alloc::Bytes_8: return alloc_counts_64.add_allocation(n_bytes / 8, tag);

        case mem::Alloc::STBI: return alloc_counts_stbi.add_allocation(n_bytes, stbi_tag);

        default: return alloc_counts_8.add_allocation(n_bytes, tag);
        }
    }


    void free_allocation(void* ptr, mem::Alloc type)
    {
        switch (type)
        {
        case mem::Alloc::STBI:
            alloc_counts_stbi.remove_allocation(ptr);
            break;

        default:
            free_allocation(ptr, (u32)type);
            break;
        }

        
    }
    

    static void set_status(AllocCounts const& src, mem::AllocationStatus& dst)
    {
        dst.type_name = src.type_name;
        dst.element_size = src.element_size;

        dst.bytes_allocated = src.bytes_allocated;
        dst.elements_allocated = src.elements_allocated;
        dst.n_allocations = src.n_allocations;

        counts::status_tags.clear();
        counts::status_sizes.clear();

        u32 d = 0;
        for (auto& item : src.list)
        {
            counts::status_tags.push_back(item.second.tag);
            counts::status_sizes.push_back(item.second.n_bytes);
        }

        dst.slot_tags = counts::status_tags.data();
        dst.slot_sizes = counts::status_sizes.data();
    }
    

    static void set_history(AllocCounts const& src, mem::AllocationHistory& dst)
    {
        dst.type_name = src.type_name;
        dst.element_size = src.element_size;
        dst.max_bytes = src.max_bytes;

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

        switch (type)
        {
        case Alloc::Bytes_1: counts::set_status(counts::alloc_counts_8, status); break;
        case Alloc::Bytes_2: counts::set_status(counts::alloc_counts_16, status); break;
        case Alloc::Bytes_4: counts::set_status(counts::alloc_counts_32, status); break;
        case Alloc::Bytes_8: counts::set_status(counts::alloc_counts_64, status); break;
        case Alloc::STBI: counts::set_status(counts::alloc_counts_stbi, status); break;
        default: counts::set_status(counts::alloc_counts_8, status); break;
        }

        return status;
    }


    AllocationHistory query_history(Alloc type)
    {
        AllocationHistory history{};        

        switch (type)
        {
        case Alloc::Bytes_1: counts::set_history(counts::alloc_counts_8, history); break;
        case Alloc::Bytes_2: counts::set_history(counts::alloc_counts_16, history); break;
        case Alloc::Bytes_4: counts::set_history(counts::alloc_counts_32, history); break;
        case Alloc::Bytes_8: counts::set_history(counts::alloc_counts_64, history); break;
        case Alloc::STBI: counts::set_history(counts::alloc_counts_stbi, history); break;
        default: counts::set_history(counts::alloc_counts_8, history); break;
        }

        return history;
    }
}

#endif