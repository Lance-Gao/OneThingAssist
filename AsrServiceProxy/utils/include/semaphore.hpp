#ifndef UTILS_SAMPAPHORE_HPP
#define UTILS_SAMPAPHORE_HPP

#include <pthread.h>

class CSemaphore {
public:
    CSemaphore(unsigned long l_init_count, unsigned long max_count);
    virtual ~CSemaphore(void);
public:
    unsigned long signal();
    int wait(long time_out);

private:
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
    int _count;
    int _max_count;
};

#endif // UTILS_SAMPAPHORE_HPP
