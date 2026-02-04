#pragma once

#include "../../../libs/io/input/input.hpp"
#include "../../../libs/image/image.hpp"


namespace game_punk
{
    namespace img = image;
    using Input = input::Input;


    constexpr auto APP_TITLE = "Punk Run";
    constexpr auto VERSION = "0.4.1";
    constexpr auto DATE = "2026-02-03";


    class StateData;


    class AppState
    {
    public:
        img::ImageView screen;

        StateData* data_ = 0;
    };


    enum class AppError : int
    {
        None = 0,
        Memory,
        Assets,
        ScreenDimensions,
        ScreenWidth,
        ScreenHeight
    };


    class AppResult
    {
    public:
        bool success = false;

        Vec2Du32 app_dimensions;

        AppError error = AppError::None;
    };


    AppResult init(AppState& state);

    AppResult init(AppState& state, Vec2Du32 available_dims);

    bool set_screen_memory(AppState& state, img::ImageView screen);

    void reset(AppState& state);

    void close(AppState& state);

    void update(AppState& state, Input const& input);

    cstr decode_error(AppError error);
}


/* debugging context */

namespace game_punk
{
#ifndef GAME_PUNK_RELEASE


    class DebugContext
    {
    public:

        union
        {
            b8 all = 0xFF;

            
        } layers;
        
    };


    bool set_screen_memory_dbg(AppState& state, image::ImageView screen, DebugContext& dbg);


    void update_dbg(AppState& state, input::Input const& input, DebugContext const& dbg);


#endif
}

