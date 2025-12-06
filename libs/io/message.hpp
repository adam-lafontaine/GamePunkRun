#pragma once

#include "../util/types.hpp"


namespace msg
{
    void info_dialogue(cstr title, cstr message);

    void warning_dialogue(cstr title, cstr message);

    void error_dialogue(cstr title, cstr message);


    inline void info_dialogue(cstr message)
    {
        info_dialogue("Info", message);
    }


    inline void warning_dialogue(cstr message)
    {
        warning_dialogue("Warning", message);
    }


    inline void error_dialogue(cstr message)
    {
        error_dialogue("Error", message);
    }
}