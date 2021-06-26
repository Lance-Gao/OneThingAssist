#include "include/semaphore.hpp"

CSemaphore::CSemaphore(unsigned long l_init_count, unsigned long max_count)
    : _max_count(max_count), _count(l_init_count) {
    pthread_mutex_init(&_lock, NULL);
    pthread_cond_init(&_cond, NULL);
}

CSemaphore::~CSemaphore(void) {
    pthread_mutex_destroy(&_lock);
    pthread_cond_destroy(&_cond);
}

unsigned long CSemaphore::signal() {
    unsigned long ret = 0;
    pthread_mutex_lock(&_lock);
    ret = ++_count;
    pthread_cond_signal(&_cond);
    pthread_mutex_unlock(&_lock);

    return ret;
}

int CSemaphore::wait(long time_out) {
    int ret = 0;
    struct timespec wait_tm = {0, 0};

    wait_tm.tv_sec = time(NULL);
    wait_tm.tv_sec += time_out / 1000;
    wait_tm.tv_nsec += (time_out % 1000) * 1000000;

    pthread_mutex_lock(&_lock);

    if (_count > 0) {
        _count--;
        ret++;
        pthread_mutex_unlock(&_lock);
        return ret;
    }

    if (time_out >= 0) {
        pthread_cond_timedwait(&_cond, &_lock, &wait_tm);
    } else {
        pthread_cond_wait(&_cond, &_lock);
    }

    if (_count > 0) {
        _count--;
        ret++;
    }

    pthread_mutex_unlock(&_lock);

    return ret;
}
