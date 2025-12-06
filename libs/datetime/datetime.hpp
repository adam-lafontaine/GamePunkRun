#pragma once

#include "../util/types.hpp"

namespace datetime
{
    constexpr u32 MILLI = 1000;
    constexpr u32 MICRO = MILLI * 1000;
    constexpr u32 NANO = MICRO * 1000;
    
    
    u64 query_nanoseconds_u64();

    u64 query_performance_counter_u64();

    i64 current_timestamp_i64();

    void delay_milli(u64 milli);

    void delay_nano(u64 nano);
}


namespace datetime
{
    class StopwatchNS // TODO: delete
    {
    private:
        u64 start_ = 0;

        u64 now() { return query_nanoseconds_u64(); }


    public:
        StopwatchNS() { start_ = now(); }

        void start() { start_ = now(); }

        u64 get_time_nano() { return now() - start_; }

        f64 get_time_nano_f64() { return (f64)get_time_nano(); }

    };


    class Stopwatch
    {
    private:
        u64 start_ = 0;

        u64 now() { return query_nanoseconds_u64(); }


    public:
        Stopwatch() { start_ = now(); }

        void start() { start_ = now(); }

        u64 get_time_nano() { return now() - start_; }

        f64 get_time_nano_f64() { return (f64)get_time_nano(); }

        u64 get_time_milli() { return get_time_nano() * MILLI / NANO; }

        f64 get_time_milli_f64() { return get_time_nano_f64() * MILLI / NANO; }

    };
}


namespace datetime
{
    class StopwatchPC
    {
    private:
        u64 start_ = 0;

        u64 now() { return query_performance_counter_u64(); }

    public:
        StopwatchPC() { start_ = now(); }

        void start() { start_ = now(); }

        u64 get_counter() { return now() - start_; }
    
    };
}