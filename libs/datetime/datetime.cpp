#include "datetime.hpp"

#include <chrono>
#include <thread>


namespace datetime
{
    u64 query_nanoseconds_u64()
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();

        return (u64)ns;
    }

    u64 query_performance_counter_u64()
    {
        return query_nanoseconds_u64();
    }


    i64 current_timestamp_i64()
    {
        auto now = std::chrono::system_clock::now();
        auto ns = duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

        return (i64)ns;
    }


    void delay_milli(u64 milli)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milli));
    }


    void delay_nano(u64 nano)
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(nano));
    }
}