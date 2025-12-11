
#ifdef APP_ASSERT_LOG

#include <SDL3/SDL.h>
#define app_assert(condition) SDL_assert(condition)
#define app_log(...) SDL_Log(__VA_ARGS__)
#define app_crash(message) SDL_assert(false && message)

#else

#define app_assert(...)
#define app_log(...)
#define app_crash(...)

#endif


#ifndef NDEBUG
#include "../../../src/app/app.cpp"
#endif

#include "../../../../libs/sdl3/sdl_span.cpp"
#include "../../../../libs/sdl3/sdl_alloc.cpp"
#include "../../../../libs/sdl3/sdl_math.cpp"
#include "../../../../libs/image/image.cpp"
#include "../../../../libs/sdl3/sdl_input.cpp"
#include "../../../../libs/sdl3/sdl_window.cpp"
#include "../../../../libs/sdl3/sdl_audio.cpp"
#include "../../../../libs/sdl3/sdl_filesystem.cpp"
#include "../../../../libs/sdl3/sdl_datetime.cpp"
#include "../../../../libs/sdl3/sdl_message.cpp"
#include "../../../../libs/sdl3/sdl_stb_libs.cpp"

#ifdef NDEBUG
#include "../../../src/app/app.cpp"
#endif