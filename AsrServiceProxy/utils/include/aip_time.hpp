#ifndef UTILS_AIP_TIME_HPP
#define UTILS_AIP_TIME_HPP

#include "aip_common.hpp"

inline long long process_time_ns() {
    timespec now;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
    return now.tv_sec * 1000000000LL + now.tv_nsec;
}

inline long long process_time_us() {
    timespec now;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
    return now.tv_sec * 1000000LL + now.tv_nsec / 1000L;
}

inline long long process_time_ms() { return process_time_ns() / 1000000L; }

inline long long process_time_s() { return process_time_ns() / 1000000000L; }

inline long long monotonic_time_ns() {
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000000000LL + now.tv_nsec;
}
inline long long monotonic_time_us() { return monotonic_time_ns() / 1000L; }

inline long long monotonic_time_ms() { return monotonic_time_ns() / 1000000L; }

inline long long monotonic_time_s() { return monotonic_time_ns() / 1000000000L; }

inline long gettimeofday_us() {
    timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000000L + now.tv_usec;
}

inline long long gettimeofday_ms() {
    timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000LL + now.tv_usec / 1000LL;
}

inline long gettimeofday_s() {
    timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec + now.tv_usec / 1000000L;
}

#endif // UTILS_AIP_TIME_HPP
