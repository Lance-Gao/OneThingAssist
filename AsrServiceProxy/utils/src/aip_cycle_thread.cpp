#include <errno.h>
#include "include/aip_cycle_thread.hpp"
#include "include/aip_log.hpp"
#include "include/aip_time.hpp"

CycleThread::CycleThread()
    : IThread(), _stop(false) {
    int ret = 0;

    ret = pthread_mutex_init(&_lock, NULL);
    if (0 != ret) {
        AIP_LOG_FATAL("pthread_mutex_init error. [err:%d]", ret);
    }

    ret = pthread_cond_init(&_cond, NULL);
    if (0 != ret) {
        AIP_LOG_FATAL("pthread_cond_init error. [err:%d]", ret);
    }
}

CycleThread::~CycleThread() {
    int ret = 0;

    ret = pthread_mutex_destroy(&_lock);
    if (0 != ret) {
        AIP_LOG_FATAL("pthread_mutex_destroy error. [err:%d]", ret);
    }

    ret = pthread_cond_destroy(&_cond);
    if (0 != ret) {
        AIP_LOG_FATAL("pthread_cond_destroy error. [err:%d]", ret);
    }
}

int CycleThread::signal() {
    int ret = 0;

    pthread_mutex_lock(&_lock);
    ret = pthread_cond_signal(&_cond);
    if (0 != ret) {
        AIP_LOG_FATAL("pthread_cond_signal error. [ret:%d]", ret);
    }
    pthread_mutex_unlock(&_lock);

    return ret;
}

int CycleThread::stop() {
    _stop = true;
    return signal();
}

int CycleThread::on_start() {
    return 0;
}

int CycleThread::on_stop() {
    return 0;
}

int CycleThread::run() {
    int ret = 0;
    int64_t start_time = 0;
    int64_t end_time = 0;
    int64_t sleep_time = 0;

    on_start();

    while (!_stop) {
        ret = run_one_cycle();
    }
    on_stop();
    AIP_LOG_NOTICE("%s exit. [tid:%d] [pthread_self:%ld]", get_thread_name(),
                    (int)(long) get_tid(), pthread_self());
    IThread::exit(0);
    return ret;
}
