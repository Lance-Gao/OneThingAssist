#include <sys/time.h>

#include "include/critsec.hpp"

CCritSec::CCritSec() {
    pthread_mutex_init(&_lock, NULL);
    pthread_cond_init(&_cond, NULL);
}

CCritSec::~CCritSec() {
    pthread_cond_destroy(&_cond);
    pthread_mutex_destroy(&_lock);
}

void CCritSec::lock() {
    pthread_mutex_lock(&_lock);
}

void CCritSec::unlock() {
    pthread_mutex_unlock(&_lock);
}

int CCritSec::signal() {
    int ret = 0;

    ret = pthread_cond_signal(&_cond);

    return ret;
}

int CCritSec::timewait(int timeout_ms) {
    int ret = 0;
    struct timespec timeout;
    struct timeval now;

    gettimeofday(&now, NULL);
    int nsec = now.tv_usec * 1000 + (timeout_ms % 1000) * 1000000;
    timeout.tv_nsec = nsec % 1000000000;
    timeout.tv_sec = now.tv_sec + nsec / 1000000000 + timeout_ms / 1000;

    ret = pthread_cond_timedwait(&_cond, &_lock, &timeout);

    return ret;
}

int CCritSec::wait() {
    int ret = 0;

    ret = pthread_cond_wait(&_cond, &_lock);

    return ret;
}
