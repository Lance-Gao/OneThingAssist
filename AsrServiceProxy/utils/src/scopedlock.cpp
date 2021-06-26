#include "include/scopedlock.hpp"

CScopedLock::CScopedLock(CCritSec* cs)
    : _cs(cs) {
    _cs->lock();
}

int CScopedLock::cscopedlock_signal() {
    int ret = 0;

    ret = _cs->signal();

    return ret;
}

int CScopedLock::cscopedlock_timewait(int timeout_ms) {
    int ret = 0;

    ret = _cs->timewait(timeout_ms);

    return ret;
}

int CScopedLock::cscopedlock_wait() {
    int ret = 0;

    ret = _cs->wait();

    return ret;
}

CScopedLock::~CScopedLock() {
    _cs->unlock();
}