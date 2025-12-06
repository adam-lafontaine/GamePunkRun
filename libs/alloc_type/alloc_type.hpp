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


/* type allocator */
#if 0
namespace mem
{
    template <typename T>
    inline constexpr bool verify_size()
    {
        auto sz = sizeof(T);

        return 
            sz == 1 ||
            sz == 2 ||
            sz == 4 ||
            sz == 8 ||
            sz == 16;
    }


    class TypeAllocator
    {
    private:
    
        struct s128{ u32 a; u32 b; u32 c; u32 d; };

        u8* u8_buffer = 0;
        u16* u16_buffer = 0;
        u32* u32_buffer = 0;
        u64* u64_buffer = 0;
        s128* u128_buffer = 0;

        u32 cap_8 = 0;
        u32 cap_16 = 0;
        u32 cap_32 = 0;
        u32 cap_64 = 0;
        u32 cap_128 = 0;

        u32 off_8 = 0;
        u32 off_16 = 0;
        u32 off_32 = 0;
        u32 off_64 = 0;
        u32 off_128 = 0;


        void adjust_size_elements(u64& size, u32& n_elements)
        {
            auto bytes = size * n_elements;

            u64 s = 1;

            if (size % 16 == 0)
            {
                s = 16;
            }
            if (size % 8 == 0)
            {
                s = 8;
            }
            else if (size % 4 == 0)
            {
                s = 4;
            }
            else if (size % 2 == 0)
            {
                s = 2;
            }

            size = s;
            n_elements = bytes / size;
            
        }


        void add_size_count(u64 size, u32 n_elements)
        {
            adjust_size_elements(size, n_elements);

            switch (size)
            {
            case 1: cap_8 += n_elements; break;
            case 2: cap_16 += n_elements; break;
            case 4: cap_32 += n_elements; break;
            case 8: cap_64 += n_elements; break;
            case 16: cap_128 += n_elements; break;
            default:
                // no other sizes allowed
                assert(false && "*** Invalid size ***");
                break;
            }
        }


        template <typename T>
        bool create_buffer(T*& buffer, u32 n_elements, cstr tag)
        {
            auto data = mem::alloc<T>(n_elements, tag);
            if (!data)
            {
                return false;
            }

            buffer = data;
            return true;
        }


        template <typename T>
        T* push_elements(T* buffer, u32 n_elements, u32 cap, u32& offset)
        {
            assert(offset < cap && "*** buffer full ***");

            if (offset + n_elements > cap)
            {
                return 0;
            }

            auto data = buffer + offset;
            offset += n_elements;
            return data;
        }


        u8* push_size_count(u64 size, u32 n_elements)
        {
            adjust_size_elements(size, n_elements);

            switch (size)
            {
            case 1:  return (u8*)push_elements(u8_buffer,   n_elements, cap_8,   off_8);
            case 2:  return (u8*)push_elements(u16_buffer,  n_elements, cap_16,  off_16);
            case 4:  return (u8*)push_elements(u32_buffer,  n_elements, cap_32,  off_32);
            case 8:  return (u8*)push_elements(u64_buffer,  n_elements, cap_64,  off_64);
            case 16: return (u8*)push_elements(u128_buffer,  n_elements, cap_128,  off_128);
            default:
                // no other sizes allowed
                assert(false && "*** Invalid size ***");
                break;
            }

            return 0;
        }

        
        template <typename T>
        void add_type_count(u32 n_elements)
        {
            //static_assert(verify_size<T>());
            auto size = sizeof(T);
            add_size_count(size, n_elements);
        }


        template <typename T>
        T* push_type_count(u32 n_elements)
        {
            //static_assert(verify_size<T>());
            auto size = sizeof(T);
            return (T*)push_size_count(size, n_elements);
        }


    public:

        template <typename T>
        void reserve(T* ptr, u32 n_elements)
        {
            add_type_count<T>(n_elements);
        }


        bool create(cstr tag)
        {
            bool ok = true;
            if (cap_8)   { ok &= create_buffer(u8_buffer,   cap_8,   tag); }
            if (cap_16)  { ok &= create_buffer(u16_buffer,  cap_16,  tag); }
            if (cap_32)  { ok &= create_buffer(u32_buffer,  cap_32,  tag); }
            if (cap_64)  { ok &= create_buffer(u64_buffer,  cap_64,  tag); }
            if (cap_128) { ok &= create_buffer(u128_buffer, cap_128, tag); }

            return ok;
        }


        template <typename T>
        bool allocate(T*& dst, u32 n_elements)
        {
            T* data = 0;

            data = push_type_count<T>(n_elements);

            if (!data)
            {
                return false;
            }

            dst = data;
            return true;
        }


        void destroy()
        {
            free(u8_buffer);
            free(u16_buffer);
            free(u32_buffer);
            free(u64_buffer);
            free(u128_buffer);
        }
    };
}
#endif


#ifdef ALLOC_COUNT

namespace mem
{ 

    struct AllocationStatus
    {
        static constexpr u32 MAX_SLOTS = 50;

        cstr type_name = 0;
        u32 element_size = 0;
        u32 max_allocations = 0;

        u32 bytes_allocated = 0;
        u32 elements_allocated = 0;

        u32 n_allocations = 0;

        // TODO: max allocations
        cstr slot_tags[MAX_SLOTS] = { 0 };
        u32 slot_sizes[MAX_SLOTS] = { 0 };
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