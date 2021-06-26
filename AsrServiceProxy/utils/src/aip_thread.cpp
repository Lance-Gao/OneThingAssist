#include <string>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "include/aip_thread.hpp"
#include "include/aip_log.hpp"

IThread::IThread() {
    _tid = 0;
    snprintf(_thread_name, MAX_NAME, "%s", "IThread");
}

IThread::~IThread() {
}

int IThread::start() {
    int ret = 0;

    ret = pthread_create(&_tid, NULL, IThread::_process, (void *)this);
    if (0 != ret) {
        AIP_LOG_FATAL("create thread error.");
        ret = -1;
    }

    return ret;
}

void *IThread::_process(void *param) {
    IThread *thread = reinterpret_cast<IThread *>(param);
    thread->run();

    return 0;
}

int IThread::detach() {
    int ret = 0;

    ret = pthread_detach(get_tid());
    if (0 != ret) {
        AIP_LOG_WARNING("detach thread error.");
        ret = -1;
    }

    return ret;
}

int IThread::join() {
    int ret = 0;

    ret = pthread_join(get_tid(), NULL);
    if (0 != ret) {
        AIP_LOG_WARNING("join thread error. [ret:%d]", ret);
        ret = -1;
    }

    return ret;
}

int IThread::exit(void *param) {
    pthread_exit(param);
}

int IThread::set_thread_name(const char *thread_name) {
    int ret = 0;
    
    if (NULL == thread_name) {
        ret = -1;
        AIP_LOG_WARNING("parameter is invalid.");
    } else {
        snprintf(_thread_name, MAX_NAME, "%s", thread_name);
    }

    return ret;
}
