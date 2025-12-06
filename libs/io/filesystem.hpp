#pragma once

#include "../span/span.hpp"


/* definitions */

namespace fs
{
    namespace sb = stack_buffer;

    using FilePathCallback = fn<void(cstr)>;


    //typedef void (*FilePathCallback)(cstr);

    
    class SingleFileResult
    {
    public:
        bool ok = false;
        
        StringView filepath;
        StringView directory;

        FilePathCallback callback;
    };    

    
}


/* inline */

namespace fs
{
    inline cstr get_file_name(cstr full_path)
    {
        auto str = span::to_string_view(full_path);
        auto c = str.data + str.length;

        for (; c >= str.data && *c != '/'; c--)
        { }

        return (cstr)(c + 1); // no leading '/'
        //return (cstr)c; // keep leading '/'
    }


    inline void copy_file_name(cstr full_path, StringView& dst)
    {
        auto name = get_file_name(full_path);

        span::sprintf(dst, "%s", name);
    }


    inline void copy_directory_path(cstr full_path, StringView& dst)
    {
        auto str = span::to_string_view(full_path);
        auto c = str.data + str.length;

        for (; c >= str.data && *c != '/'; c--)
        { }

        auto len = c - str.data + 1; // no trailing '/'

        span::sprintf(dst, len, "%s", full_path);
    }


    /*template <u32 N>
    inline StringView get_file_name(cstr full_path, StackBuffer<u8, N>& buffer)
    {
        auto str = span::to_string_view(full_path);
        auto c = str.data + str.length;

        for (; c >= str.data && *c != '/'; c--)
        { }

        u32 len = str.length - (c - str.data);

        auto sv = span::make_view(len, buffer);

        return sv;
    }*/
}


/* api */

namespace fs
{

    u32 file_size(cstr file_path);

    MemoryBuffer<u8> read_bytes(cstr path);

    void select_image_file(SingleFileResult* result);
}