#include "../io/filesystem.hpp"
#include "../alloc_type/alloc_type.hpp"
#include "sdl_include.hpp"

#include <filesystem>


#define ASSERT_FILESYSTEM
#define LOG_FILESYSTEM


#ifndef NDEBUG

#ifdef LOG_FILESYSTEM
#define fs_log(...) SDL_Log(__VA_ARGS__)
#else
#define fs_log(...)
#endif

#ifdef ASSERT_FILESYSTEM
#define fs_assert(condition) SDL_assert(condition)
#else
#define fs_assert(...)
#endif

#else

#define fs_log(...)
#define fs_assert(...)

#endif


namespace fs
{
    namespace mb = memory_buffer;
    namespace sfs = std::filesystem;
}


namespace fs
{
    u32 file_size(cstr file_path)
    {
        auto size = sfs::file_size(file_path);
        
        fs_assert("*** file size zero ***" && size);
        
        return (u32)size;
    }


    MemoryBuffer<u8> read_bytes(cstr file_path)
    {
        MemoryBuffer<u8> buffer;
        buffer.ok = 0;
    
    
    #ifndef __EMSCRIPTEN__        

        u64 size = 0;

        auto data = SDL_LoadFile(file_path, &size);
        if (!data)
        {
            fs_log("SDL_LoadFile() error (%s): %s", file_path, SDL_GetError());
            //fs_assert("*** SDL_LoadFile() error ***" && false);
            return buffer;
        }

        buffer.data_ = (u8*)data;
        buffer.capacity_ = (u32)size;
        buffer.ok = 1;

        mem::add_allocation(buffer.data_, buffer.size_, get_file_name(file_path));

    #else

        fs_assert(false && "*** There is no filesystem on the web ***");
    #endif

        return buffer;
    }


    void select_image_file(SingleFileResult* result)
    {
        fs_assert("select_image_file() no implemented" && false);
    }

}