#pragma once


/* memory allocation */

namespace game_punk
{

    class MemoryCounts
    {
    public:
        u32 count_8 = 0;
        u32 count_16 = 0;
        u32 count_32 = 0;
        u32 count_64 = 0;
        u32 count_misc = 0;
    };


    template <typename T>
    void add_count(MemoryCounts& mc, u32 n)
    {
        auto s = sizeof(T);

        switch (s)
        {
        case sizeof(u8): mc.count_8 += n; break;
        case sizeof(u16): mc.count_16 += n; break;
        case sizeof(u32): mc.count_32 += n; break;
        case sizeof(u64): mc.count_64 += n; break;
        default: mc.count_misc += s * n; break;
        }
    }


    class Memory
    {
    public:
        bool ok = 0;

        Buffer8 mem_8;
        Buffer16 mem_16;
        Buffer32 mem_32;
        Buffer64 mem_64;
        Buffer8 mem_misc;
    };


    static void destroy_memory(Memory& mem)
    {
        mb::destroy_buffer(mem.mem_8);
        mb::destroy_buffer(mem.mem_16);
        mb::destroy_buffer(mem.mem_32);
        mb::destroy_buffer(mem.mem_64);
        mb::destroy_buffer(mem.mem_misc);

        mem.ok = false;
    }


    static Memory create_memory(MemoryCounts const& counts)
    {
        Memory mem{};
        mem.ok = false;

        bool ok = true;

        auto create = [&](auto& buffer, auto& count, cstr tag) 
        { 
            if (count)
            {
                ok &= mb::create_buffer(buffer, count, tag); 
            }

            if (count && ok)
            {
                mb::zero_buffer(buffer);
            }
        };

        create(mem.mem_8, counts.count_8, "8");
        create(mem.mem_16, counts.count_16, "16");
        create(mem.mem_32, counts.count_32, "32");
        create(mem.mem_64, counts.count_64, "64");
        create(mem.mem_misc, counts.count_misc, "misc");

        if (!ok)
        {
            destroy_memory(mem);
            return mem;
        }
        
        mem.ok = true;

        return mem;
    }


    template <typename T>
    static Result<T*> push_mem(Memory& mem, u32 n_elements)
    {
        Result<T*> res{};
        res.ok = false;

        auto s = sizeof(T);

        switch (s)
        {
        case sizeof(u8): res.data = (T*)mb::push_elements(mem.mem_8, n_elements); break;
        case sizeof(u16): res.data = (T*)mb::push_elements(mem.mem_16, n_elements); break;
        case sizeof(u32): res.data = (T*)mb::push_elements(mem.mem_32, n_elements); break;
        case sizeof(u64): res.data = (T*)mb::push_elements(mem.mem_64, n_elements); break;
        default: res.data = (T*)mb::push_elements(mem.mem_misc, s * n_elements); break;
        }

        if (res.data)
        {
            res.ok = true;
        }

        return res;
    }


    static void log_mem(Memory const& mem, MemoryCounts const& mc)
    {
        auto const write = [](cstr name, auto const& mem, u32 count)
        {
            app_log("%s(%u): %u / %u / %u\n", name, (u32)sizeof(mem.data_[0]), mem.size_, mem.capacity_, count); 
        };

        app_log("\n");
        write("     8", mem.mem_8, mc.count_8);
        write("    16", mem.mem_16, mc.count_16);
        write("    32", mem.mem_32, mc.count_32);
        write("    64", mem.mem_64, mc.count_64);
        write("  misc", mem.mem_misc, mc.count_misc);
        app_log("\n");
    }
    
}


/* type allocations */

namespace game_punk
{   
    template <typename T>
    static void count_span(SpanView<T>& view, MemoryCounts& counts, u32 length)
    {
        view.length = length;

        add_count<T>(counts, length);
    }


    template <typename T>
    static bool create_span(SpanView<T>& view, Memory& mem)
    {
        if (!view.length)
        {
            app_assert("SpanView not initialized" && false);
            return false;
        }

        auto res = push_mem<T>(mem, view.length);
        if (res.ok)
        {
            view.data = res.data;
        }

        return res.ok;
    }
        
    
    static void count_view(ImageView& view, MemoryCounts& counts, u32 width, u32 height)
    {
        view.width = width;
        view.height = height;

        add_count<p32>(counts, width * height);
    }


    static bool create_view(ImageView& view, Memory& mem)
    {
        auto n_pixels = view.width * view.height;
        if (!n_pixels)
        {
            app_assert("ImageView not initialized" && false);
            return false;
        }

        auto res = push_mem<p32>(mem, n_pixels);
        if (res.ok)
        {
            view.matrix_data_ = res.data;
        }

        return res.ok;
    }
    
}
