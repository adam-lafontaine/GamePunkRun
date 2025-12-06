#include "../datetime/datetime.hpp"

#include <SDL3/SDL.h>


namespace datetime
{
    u64 query_nanoseconds_u64()
    {
        return SDL_GetTicksNS();
    }


    u64 query_performance_counter_u64()
    {
        return SDL_GetPerformanceCounter();
    }


    i64 current_timestamp_i64()
    {
        SDL_Time ticks = 0;
        int res = (int)SDL_GetCurrentTime(&ticks);

        return ticks * res;
    }


    void delay_milli(u64 milli)
    {
        SDL_Delay(milli);
    }


    void delay_nano(u64 nano)
    {
        SDL_DelayNS(nano);
    }    
    
}