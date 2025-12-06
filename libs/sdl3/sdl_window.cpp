#pragma once

#include "../io/window.hpp"
#include "../alloc_type/alloc_type.hpp"
#include "sdl_include.hpp"


/* screen memory */

namespace sdl
{
    class ScreenMemory
    {
    public:
        SDL_Window* window = 0;
        SDL_Renderer* renderer = 0;
        SDL_Texture* texture = 0;

        SDL_FRect render_rect;

        u32 width_px = 0;
        u32 height_px = 0;
    };


    static void destroy_screen_memory(ScreenMemory& screen)
    {
        if (screen.texture)
        {
            SDL_DestroyTexture(screen.texture);
        }

        if (screen.renderer)
        {
            SDL_DestroyRenderer(screen.renderer);
        }

        if(screen.window)
        {
            SDL_DestroyWindow(screen.window);
        }

        SDL_zero(screen);
    }


    static bool create_window(ScreenMemory& screen, cstr title, u32 width, u32 height)
    {
        screen.window = SDL_CreateWindow(
            title,
            (int)width,
            (int)height,
            SDL_WINDOW_RESIZABLE);

        if(!screen.window)
        {
            display_error("SDL_CreateWindow failed");
            return false;
        }

        return true;
    }


    static bool create_window_fullscreen(ScreenMemory& screen, cstr title)
    {
        screen.window = SDL_CreateWindow(
            title,
            0,
            0,
            SDL_WINDOW_FULLSCREEN);

        if(!screen.window)
        {
            display_error("SDL_CreateWindow failed");
            return false;
        }

        return true;
    }


    static bool create_renderer(ScreenMemory& screen)
    {
        screen.renderer = SDL_CreateRenderer(screen.window, NULL);

        if(!screen.renderer)
        {
            display_error("SDL_CreateRenderer failed");
            return false;
        }

        return true;
    }


    static bool create_texture(ScreenMemory& screen, u32 width, u32 height)
    {
        static_assert(window::PIXEL_SIZE == 4); // SDL_PIXELFORMAT_ABGR8888

        screen.texture =  SDL_CreateTexture(
            screen.renderer,
            SDL_PIXELFORMAT_ABGR8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height);
        
        if(!screen.texture)
        {
            display_error("SDL_CreateTexture failed");
            return false;
        }

        screen.width_px = width;
        screen.height_px = height;

        return true;
    }


    static bool create_screen_memory(ScreenMemory& screen, cstr title, Vec2Du32 window_size, Vec2Du32 pixel_size)
    {
        destroy_screen_memory(screen);

        if (!create_window(screen, title, window_size.x, window_size.y))
        {
            destroy_screen_memory(screen);
            return false;
        }
        
        if (!create_renderer(screen))
        {
            destroy_screen_memory(screen);
            return false;
        }

        if (!create_texture(screen, pixel_size.x, pixel_size.y))
        {
            destroy_screen_memory(screen);
            return false;
        }       

        return true;
    }


    static bool create_screen_memory_fullscreen(ScreenMemory& screen, cstr title, Vec2Du32 pixel_size)
    {
        destroy_screen_memory(screen);

        if (!create_window_fullscreen(screen, title))
        {
            destroy_screen_memory(screen);
            return false;
        }
        
        if (!create_renderer(screen))
        {
            destroy_screen_memory(screen);
            return false;
        }

        int width = 0;
        int height = 0;
        
        if (!SDL_GetCurrentRenderOutputSize(screen.renderer, &width, &height))
        {
            destroy_screen_memory(screen);
            return false;
        }

        if (!create_texture(screen, pixel_size.x, pixel_size.y))
        {
            destroy_screen_memory(screen);
            return false;
        }

        return true;
    }
}


/* static data */

namespace sdl
{
    constexpr u32 N_SCREEN_MEMORY = 2;

    static ScreenMemory screen_data[N_SCREEN_MEMORY] = { 0 };
    static u32 screen_data_size = 0;
}


/* sdl helpers */

namespace sdl
{    
    static ScreenMemory* allocate_screen_memory()
    {
        if (screen_data_size >= N_SCREEN_MEMORY)
        {
            return 0;
        }

        auto data = screen_data + screen_data_size;
        ++screen_data_size;

        return data;
    }


    static void resize_render_rect(ScreenMemory& screen)
    {
        bool ok = true;

        ok = SDL_SetRenderDrawColor(screen.renderer, 0, 0, 0, 255); // Black background
        ok = SDL_RenderClear(screen.renderer);

        int out_width;
        int out_height;

        ok = SDL_GetCurrentRenderOutputSize(screen.renderer, &out_width, &out_height);

    #ifdef PRINT_MESSAGES
        if (!ok)
        {
            print_error("SDL_GetCurrentRenderOutputSize()");
            return;
        }
    #endif

        auto in_width = (int)screen.width_px;
        auto in_height = (int)screen.height_px;

        auto scale_w = (f32)out_width / in_width;
        auto scale_h = (f32)out_height / in_height;

        auto scale = scale_w < scale_h ? scale_w : scale_h;

        auto w = (int)(scale * in_width);
        auto h = (int)(scale * in_height);

        auto& r = screen.render_rect;

        r.x = (out_width - w) / 2;
        r.y = (out_height - h) / 2;
        r.w = w;
        r.h = h;
    }


    static void resize_render_rect(ScreenMemory& screen, window::Rotate rotate)
    {
        using R = window::Rotate;

        bool ok = true;

        ok = SDL_SetRenderDrawColor(screen.renderer, 0, 0, 0, 255); // Black background
        ok = SDL_RenderClear(screen.renderer);

        int out_width;
        int out_height;

        ok = SDL_GetCurrentRenderOutputSize(screen.renderer, &out_width, &out_height);

    #ifdef PRINT_MESSAGES
        if (!ok)
        {
            print_error("SDL_GetCurrentRenderOutputSize()");
            return;
        }
    #endif

        auto in_width = (int)screen.width_px;
        auto in_height = (int)screen.height_px;

        auto scale_w = (f32)out_width / in_width;
        auto scale_h = (f32)out_height / in_height;

        switch (rotate)
        {
        case R::Clockwise_90:
        case R::CounterClockwise_90:
            scale_w = (f32)out_width / in_height;
            scale_h = (f32)out_height / in_width;
            break;
        default:
            break;
        }

        auto scale = scale_w < scale_h ? scale_w : scale_h;

        auto w = (int)(scale * in_width);
        auto h = (int)(scale * in_height);

        auto& r = screen.render_rect;

        r.x = (out_width - w) / 2;
        r.y = (out_height - h) / 2;
        r.w = w;
        r.h = h;
    }

}


/* window helpers */

namespace window
{    
    static bool create_window_memory(Window& window, cstr title, Vec2Du32 window_size, Vec2Du32 pixel_size)
    {
        auto data = sdl::allocate_screen_memory();
        if (!data)
        {
            return false;
        }

        auto& screen = *data;

        if (!sdl::create_screen_memory(screen, title, window_size, pixel_size))
        {
            return false;
        }

        window.handle = (u64)data;

        return true;
    }


    static bool create_window_memory_fullscreen(Window& window, cstr title, Vec2Du32 pixel_size)
    {
        auto data = sdl::allocate_screen_memory();
        if (!data)
        {
            return false;
        }

        auto& screen = *data;

        if (!sdl::create_screen_memory_fullscreen(screen, title, pixel_size))
        {
            return false;
        }

        window.handle = (u64)data;

        return true;
    }


    static sdl::ScreenMemory& get_screen(Window const& window)
    {
        return *(sdl::ScreenMemory*)window.handle;
    }
    

    static void set_window_icon_64(sdl::ScreenMemory& screen, window::Icon64 const& icon_64)
    {
        static_assert(window::PIXEL_SIZE == window::Icon64::bytes_per_pixel);

        sdl::set_window_icon(screen.window, icon_64);
    }


    static void copy_window_pixels(Window const& window, void* dst_data, int dst_pitch)
    {   
        auto h = window.height_px;
        auto w = window.width_px;

        auto dst = (u8*)dst_data;
        auto src = (u8*)window.pixel_buffer;

        u64 row_bytes = w * sizeof(u32);
        u64 row_pitch = (u64)dst_pitch;

        if (row_bytes == row_pitch)
        {
            auto length = row_bytes * h;
            SDL_memcpy(dst, src, length);
        }
        else
        {
            for (u32 y = 0; y < h; y++)
            {
                SDL_memcpy(dst, src, row_bytes);

                src += row_bytes;
                dst += dst_pitch;
            }
        }
    }


    static f32 get_rotate_angle(Rotate r)
    {
        switch (r)
        {
        case Rotate::Clockwise_90: return 90.0f;
        case Rotate::CounterClockwise_90: return -90.0f;
        default: return 0.0f;
        }
    }
}


/* api */

namespace window
{
    constexpr auto subsystem_flags = SDL_INIT_VIDEO;


    bool init()
    {
        if (!SDL_InitSubSystem(subsystem_flags))
        {
            sdl::print_error("Init Video failed");
            return false;
        }

        return true;
    }


    void close()
    {
        SDL_QuitSubSystem(subsystem_flags);
    }


    bool create(Window& window, cstr title, Vec2Du32 window_size, Vec2Du32 pixel_size)
    {
        SDL_zero(window);
        
        if (!create_window_memory(window, title, window_size, pixel_size))
        {
            return false;
        }

        auto& screen = get_screen(window);

        sdl::resize_render_rect(screen);

        auto buffer = mem::alloc<u32>(pixel_size.x * pixel_size.y, "window.pixel_buffer");
        if (!buffer)
        {
            sdl::destroy_screen_memory(screen);
            SDL_zero(window);
            return false;
        }

        window.pixel_buffer = buffer;
        window.width_px = pixel_size.x;
        window.height_px = pixel_size.y;

        return true;
    }


    bool create(Window& window, cstr title, Vec2Du32 window_size, Vec2Du32 pixel_size, Rotate rotate)
    {        
        if (rotate == Rotate::None)
        {
            sdl::print_error("Window rotate must be specified");
            return false;
        }

        SDL_zero(window);
        
        if (!create_window_memory(window, title, window_size, pixel_size))
        {
            return false;
        }

        auto& screen = get_screen(window);

        sdl::resize_render_rect(screen, rotate);

        auto buffer = mem::alloc<u32>(pixel_size.x * pixel_size.y, "window.pixel_buffer");
        if (!buffer)
        {
            sdl::destroy_screen_memory(screen);
            SDL_zero(window);
            return false;
        }

        window.pixel_buffer = buffer;
        window.width_px = pixel_size.x;
        window.height_px = pixel_size.y;

        return true;
    }


    bool create_fullscreen(Window& window, cstr title, Vec2Du32 pixel_size)
    {
        SDL_zero(window);
        
        if (!create_window_memory_fullscreen(window, title, pixel_size))
        {
            return false;
        }

        auto& screen = get_screen(window);

        sdl::resize_render_rect(screen);

        auto buffer = mem::alloc<u32>(pixel_size.x * pixel_size.y, "window.pixel_buffer");
        if (!buffer)
        {
            sdl::destroy_screen_memory(screen);
            SDL_zero(window);
            return false;
        }

        window.pixel_buffer = buffer;
        window.width_px = screen.width_px;
        window.height_px = screen.height_px;

        return true;
    }


    bool create_fullscreen(Window& window, cstr title, Vec2Du32 pixel_size, Rotate rotate)
    {
        if (rotate == Rotate::None)
        {
            sdl::print_error("Window rotate must be specified");
            return false;
        }

        SDL_zero(window);
        
        if (!create_window_memory_fullscreen(window, title, pixel_size))
        {
            return false;
        }

        auto& screen = get_screen(window);

        sdl::resize_render_rect(screen, rotate);

        auto buffer = mem::alloc<u32>(pixel_size.x * pixel_size.y, "window.pixel_buffer");
        if (!buffer)
        {
            sdl::destroy_screen_memory(screen);
            SDL_zero(window);
            return false;
        }

        window.pixel_buffer = buffer;
        window.width_px = screen.width_px;
        window.height_px = screen.height_px;

        return true;
    }


    void set_window_icon(Window& window, Icon64 const& icon)
    {
        auto& screen = get_screen(window);
        set_window_icon_64(screen, icon);
    }


    void destroy(Window& window)
    {
        auto& screen = get_screen(window);

        sdl::destroy_screen_memory(screen);
        mem::free(window.pixel_buffer);

        SDL_zero(window);
    }


    bool resize_pixel_buffer(Window& window, u32 width, u32 height)
    {
        auto& screen = get_screen(window);

        if (width == screen.width_px && height == screen.height_px)
        {
            return true;
        }
        
        if (screen.texture)
        {
            SDL_DestroyTexture(screen.texture);
            screen.texture = 0;
        }

        if (!sdl::create_texture(screen, width, height))
        {
            return false;
        }
        
        auto n_pixels = width * height;
        if (screen.width_px * screen.height_px == n_pixels)
        {            
            return true;
        }

        if (window.pixel_buffer)
        {
            mem::free(window.pixel_buffer);
            window.pixel_buffer = 0;
        }        

        auto buffer = mem::alloc<u32>(n_pixels, "window.pixel_buffer");
        if (!buffer)
        {
            return false;
        }

        window.pixel_buffer = buffer;
        window.width_px = screen.width_px;
        window.height_px = screen.height_px;

        return true;
    }


    void render(Window const& window, b32 size_changed)
    {
        bool ok = true;

        auto& screen = get_screen(window);

        if (size_changed)
        {
            sdl::resize_render_rect(screen);
        }

        ok = SDL_SetRenderDrawColor(screen.renderer, 0, 0, 0, 255); // Black background
        ok = SDL_RenderClear(screen.renderer);

        void* dst_data = 0;
        int dst_pitch = 0;

        ok = SDL_LockTexture(screen.texture, NULL, &dst_data, &dst_pitch);
        if (ok)
        {
            copy_window_pixels(window, dst_data, dst_pitch);
            SDL_UnlockTexture(screen.texture);
        }

        #ifdef PRINT_MESSAGES
        if (!ok)
        {
            sdl::print_error("SDL_LockTexture()");
        }
        #endif

        ok = SDL_RenderTexture(screen.renderer, screen.texture, NULL, &screen.render_rect);

        #ifdef PRINT_MESSAGES
        if (!ok)
        {
            sdl::print_error("SDL_RenderTexture()");
        }
        #endif
        
        SDL_RenderPresent(screen.renderer);
    }


    void render(Window const& window, Rotate rotate, b32 size_changed)
    {
        bool ok = true;

        auto& screen = get_screen(window);

        if (size_changed)
        {
            sdl::resize_render_rect(screen, rotate);
        }

        ok = SDL_SetRenderDrawColor(screen.renderer, 0, 0, 0, 255); // Black background
        ok = SDL_RenderClear(screen.renderer);

        void* dst_data = 0;
        int dst_pitch = 0;

        ok = SDL_LockTexture(screen.texture, NULL, &dst_data, &dst_pitch);
        if (ok)
        {
            copy_window_pixels(window, dst_data, dst_pitch);
            SDL_UnlockTexture(screen.texture);
        }

        #ifdef PRINT_MESSAGES
        if (!ok)
        {
            sdl::print_error("SDL_LockTexture()");
        }
        #endif

        auto angle = get_rotate_angle(rotate);

        ok = SDL_RenderTextureRotated(screen.renderer, screen.texture, 
            NULL, 
            &screen.render_rect, 
            angle, 
            NULL, 
            SDL_FLIP_NONE);

        #ifdef PRINT_MESSAGES
        if (!ok)
        {
            sdl::print_error("SDL_RenderTextureRotated()");
        }
        #endif
        
        SDL_RenderPresent(screen.renderer);
    }


    void hide_mouse_cursor()
    {
        SDL_HideCursor();
    }


    void show_mouse_cursor()
    {
        SDL_ShowCursor();
    }
}