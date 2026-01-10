#include "../alloc_type/alloc_type.hpp"

#include <SDL3/SDL.h>

#define STBI_MALLOC mem::alloc_stbi
#define STBI_REALLOC mem::realloc_stbi
#define STBI_FREE mem::free_stbi
#define STBI_ASSERT SDL_assert

#include "../stb_libs/stb_libs.cpp"