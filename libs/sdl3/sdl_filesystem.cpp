#include "../io/filesystem.hpp"
#include "../alloc_type/alloc_type.hpp"
#include "sdl_include.hpp"


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


    static constexpr SDL_DialogFileFilter example_filters[] = {
        { "PNG images",  "png" },
        { "JPEG images", "jpg;jpeg" },
        { "All images",  "png;jpg;jpeg" },
        { "All files",   "*" }
    };


    // stb_image_options.cpp
    // image.cpp
    static constexpr SDL_DialogFileFilter image_filters[] = {
        { "PNG images",  "png;PNG" },
        { "BMP images",  "bmp;BMP" },
        { "All images",  "png;PNG;bmp;BMP" },
    };
}


/* helpers */

namespace fs
{
    
    static void SDLCALL single_file_callback(void* userdata, const char* const* filelist, int filter)
    {
        auto& res = *(SingleFileResult*)userdata;
        res.ok = false;

        if (!filelist || !filelist[0])
        {
            return;
        }

        auto file = filelist[0];

        span::sprintf(res.filepath, "%s", file);
        copy_directory_path(file, res.directory);
        
        res.ok = true;

        res.callback(span::to_cstr(res.filepath));
    }


    
}


/* api */

namespace fs
{
    u32 file_size(cstr file_path)
    {
        auto file = SDL_IOFromFile(file_path, "rb");
        if (!file)
        {
            fs_log("SDL_IOFromFile error (%s): %s", file_path, SDL_GetError());
            fs_assert("*** SDL_IOFromFile error ***" && false);
            return 0;
        }

        auto size = SDL_GetIOSize(file);
        if (size < 0)
        {
            fs_log("SDL_GetIOSize error (%s): %s", file_path, SDL_GetError());
            fs_assert("*** SDL_GetIOSize error ***" && false);
            return 0;
        }

        SDL_CloseIO(file);

        return (u32)size;
    }


    MemoryBuffer<u8> read_bytes(cstr file_path)
    {
        MemoryBuffer<u8> buffer;
        buffer.ok = 0;

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
        buffer.size_ = buffer.capacity_;
        buffer.ok = 1;

        mem::add_allocation(buffer.data_, buffer.size_, get_file_name(file_path));

        return buffer;
    }


    void select_image_file(SingleFileResult* result)
    {
        auto userdata = (void*)result;
        SDL_Window* window = 0;
        auto filters = image_filters;
        constexpr int nfilters = SDL_arraysize(image_filters);
        cstr default_dir = 0;
        bool allow_many = 0;

        auto& res = *result;
        if (res.directory.length)
        {
            default_dir = span::to_cstr(res.directory);
            fs_log("default: %s\n", default_dir);
        }

        SDL_ShowOpenFileDialog(single_file_callback, userdata, window, filters, nfilters, default_dir, allow_many);
    }
}