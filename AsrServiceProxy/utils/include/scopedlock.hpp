#ifndef UTILS_SCOPEDLOCK_HPP
#define UTILS_SCOPEDLOCK_HPP

#include "critsec.hpp"

class CScopedLock {
public:
    CScopedLock(CCritSec* cs);
    ~CScopedLock();
    int cscopedlock_signal();
    int cscopedlock_timewait(int timeout_ms);
    int cscopedlock_wait();

private:
    CCritSec* _cs;
};

#endif // UTILS_SCOPEDLOCK_HPP
