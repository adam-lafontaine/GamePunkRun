#pragma once

#if defined(_WIN32)
#define SDL_MAIN_HANDLED
#endif

#include <SDL3/SDL.h>

#ifndef NDEBUG
#define PRINT_MESSAGES
#endif

#ifdef __EMSCRIPTEN__
#define NO_HAPTIC
#define NO_WINDOW
#ifdef WASM_PRINTF
#define PRINT_MESSAGES
#endif
#endif


namespace sdl
{
    inline void print_message(const char* msg)
    {
    #ifdef PRINT_MESSAGES
        SDL_Log("%s\n", msg);
    #endif
    }


    inline void print_error(const char* msg)
    {
    #ifdef PRINT_MESSAGES
        SDL_Log("%s\n%s\n", msg, SDL_GetError());
    #endif
    }


    inline void display_message(const char* msg)
    {
    #ifndef NO_WINDOW
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Message", msg, 0);
    #endif

        print_error(msg);
    }


    inline void display_error(const char* title)
    {
    #ifndef NO_WINDOW
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title, SDL_GetError(), 0);
    #endif

        print_error(title);
    }


    inline void set_window_icon(SDL_Window* window, auto const& icon_64)
    {
        SDL_Surface* icon = SDL_CreateSurfaceFrom(
            (int)icon_64.width,
            (int)icon_64.height,
            SDL_PIXELFORMAT_ABGR8888,
            (void*)icon_64.pixel_data,
            icon_64.bytes_per_pixel * icon_64.width
        );

        SDL_SetWindowIcon(window, icon);

        SDL_DestroySurface(icon);
    }
}