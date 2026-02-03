#ifdef APP_ASSERT_LOG

#ifndef NDEBUG
#include <SDL2/SDL.h>
#define app_assert(condition) SDL_assert(condition)
#define app_log(...) SDL_Log(__VA_ARGS__)
#define app_crash(message) SDL_assert(false && message)
#endif

#else

#define app_assert(...)
#define app_log(...)
#define app_crash(...)

#endif


#ifndef NDEBUG
#include "../../../src/app/app.cpp"
#endif

#include "../../../../libs/sdl2/sdl_span.cpp"
#include "../../../../libs/sdl2/sdl_alloc.cpp"
#include "../../../../libs/image/image.cpp"
#include "../../../../libs/sdl2/sdl_input.cpp"
#include "../../../../libs/sdl2/sdl_window.cpp"
#include "../../../../libs/sdl2/sdl_message.cpp"
#include "../../../../libs/sdl2/sdl_stb_libs.cpp"

#include "../../../../libs/math/math.cpp"
#include "../../../../libs/datetime/datetime.cpp"

#ifndef NO_FILESYSTEM
#include "../../../../libs/sdl2/sdl_filesystem.cpp"
#endif

#ifndef NO_AUDIO
#include "../../../../libs/sdl2/sdl_audio.cpp"
#endif

#ifdef NDEBUG
#include "../../../src/app/app.cpp"
#endif