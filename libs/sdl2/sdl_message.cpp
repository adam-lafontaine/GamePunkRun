#include "../io/message.hpp"

#include <SDL2/SDL.h>


/* helpers */

namespace msg
{
    static void simple_dialogue(cstr title, cstr message, SDL_MessageBoxFlags flag)
    {
        SDL_Window* window = 0;
        SDL_ShowSimpleMessageBox(flag, title, message, window);
    }
}


/* api */

namespace msg
{
    void info_dialogue(cstr title, cstr message)
    {
        simple_dialogue(title, message, SDL_MESSAGEBOX_INFORMATION);
    }


    void warning_dialogue(cstr title, cstr message)
    {
        simple_dialogue(title, message, SDL_MESSAGEBOX_WARNING);
    }


    void error_dialogue(cstr title, cstr message)
    {
        simple_dialogue(title, message, SDL_MESSAGEBOX_ERROR);
    }
}