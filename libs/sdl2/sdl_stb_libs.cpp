#include <SDL2/SDL.h>

#define STBI_MALLOC SDL_malloc
#define STBI_REALLOC SDL_realloc
#define STBI_FREE SDL_free
#define STBI_ASSERT SDL_assert

#include "../stb_libs/stb_libs.cpp"