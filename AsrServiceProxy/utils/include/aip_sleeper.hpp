#ifndef UTILS_AIP_SLEEPER_HPP
#define UTILS_AIP_SLEEPER_HPP

#include "aip_common.hpp"

class Sleeper {
public:
    Sleeper();

    virtual ~Sleeper();    
    virtual int sleep_s(int64_t seconds);
    virtual int sleep_ms(int64_t m_seconds);
    virtual int sleep_us(int64_t u_seconds);
    virtual int sleep_ns(int64_t n_seconds);
private:
    int _sleep(int64_t n_seconds);

private:
    timespec _sleep_time; 
};

#endif // UTILS_AIP_SLEEPER_HPP
