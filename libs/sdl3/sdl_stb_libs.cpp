#include <SDL3/SDL.h>

#define STBI_MALLOC SDL_malloc
#define STBI_REALLOC SDL_realloc
#define STBI_FREE SDL_free
#define STBI_ASSERT SDL_assert

//#define STBIR_MALLOC SDL_malloc
//#define STBIR_REALLOC SDL_realloc
//#define STBIR_FREE SDL_free
#define STBIR_ASSERT SDL_assert

#include "../stb_libs/stb_libs.cpp"