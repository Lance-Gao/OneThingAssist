#ifndef UTILS_CCRITSEC_HPP
#define UTILS_CCRITSEC_HPP

#include <pthread.h>

class CCritSec {
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
public:
    CCritSec();
    ~CCritSec();
public:
    void lock();
    void unlock();
    int signal();
    int timewait(int timeout_ms);
    int wait();
};

#endif // UTILS_CCRITSEC_HPP
