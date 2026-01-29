/* touch */

namespace sdl
{
    static void record_touch_input_event(SDL_Event const& event, Input const& prev, Input& curr)
    {
    #ifndef NO_TOUCH

        switch (event.type)
        {
        case SDL_EVENT_FINGER_DOWN:
        case SDL_EVENT_FINGER_UP:

            break;

        case SDL_EVENT_FINGER_MOTION:

            break;

        default:
            break;
        }

    #endif
    }
}