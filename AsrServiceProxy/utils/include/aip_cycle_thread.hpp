#ifndef UTILS_AIP_CYCLE_THREAD_HPP
#define UTILS_AIP_CYCLE_THREAD_HPP

#include "aip_common.hpp"
#include "aip_thread.hpp"

class CycleThread : public IThread {
public:
    CycleThread();
    ~CycleThread();
    
    int signal();
    int stop();
    virtual int run();
public:
    virtual int run_one_cycle() = 0;
    virtual int get_interval_ms() = 0;    
    virtual int on_start();
    virtual int on_stop();
private:
    bool _stop;
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
};

#endif // UTILS_AIP_CYCLE_THREAD_HPP
