#pragma once

#include "../../../libs/io/input/input.hpp"
#include "../../../libs/image/image.hpp"


namespace console_app
{
    namespace img = image;
    using Input = input::Input;


    class AppState
    {
    public:

        struct 
        {
            img::ImageView full;

            img::SubView left;
            img::SubView right;
            img::SubView center;

        } screen;


        Rect2Df32 r;


    };


}